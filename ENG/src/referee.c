#include "referee.h"
#include "string.h"
#include "stdio.h"
#include "CRC8_CRC16.h"
#include "protocol.h"

extern UART_HandleTypeDef huart6;
frame_header_t referee_receive_header;
frame_header_t referee_send_header;
uint8_t referee_tx_buf[REF_PROTOCOL_FRAME_MAX_SIZE];

ext_game_state_t game_state;
ext_game_result_t game_result;
ext_game_robot_HP_t game_robot_HP_t;

ext_event_data_t field_event;
ext_supply_projectile_action_t supply_projectile_action_t;
//ext_supply_projectile_booking_t supply_projectile_booking_t;
ext_referee_warning_t referee_warning_t;


ext_game_robot_state_t robot_state;
ext_power_heat_data_t power_heat_data_t;
ext_game_robot_pos_t game_robot_pos_t;
ext_buff_t buff_t;
aerial_robot_energy_t robot_energy_t;
ext_robot_hurt_t robot_hurt_t;
ext_shoot_data_t shoot_data_t;
ext_bullet_remaining_t bullet_remaining_t;
ext_student_interactive_header_data_t student_interactive_header_data_t;

ext_client_custom_character_t client_custom_character_t;
ext_client_custom_graphic_double_t client_custom_graphic_double_t;
custom_controller_t custom_controller_data_t;

uint8_t char_data[30];

void init_referee_struct_data(void)
{
    memset(&referee_receive_header, 0, sizeof(frame_header_t));
    memset(&referee_send_header, 0, sizeof(frame_header_t));

    memset(&game_state, 0, sizeof(ext_game_state_t));
    memset(&game_result, 0, sizeof(ext_game_result_t));
    memset(&game_robot_HP_t, 0, sizeof(ext_game_robot_HP_t));


    memset(&field_event, 0, sizeof(ext_event_data_t));
    memset(&supply_projectile_action_t, 0, sizeof(ext_supply_projectile_action_t));
//    memset(&supply_projectile_booking_t, 0, sizeof(ext_supply_projectile_booking_t));
    memset(&referee_warning_t, 0, sizeof(ext_referee_warning_t));


    memset(&robot_state, 0, sizeof(ext_game_robot_state_t));
    memset(&power_heat_data_t, 0, sizeof(ext_power_heat_data_t));
    memset(&game_robot_pos_t, 0, sizeof(ext_game_robot_pos_t));
    memset(&buff_t, 0, sizeof(ext_buff_t));
    memset(&robot_energy_t, 0, sizeof(aerial_robot_energy_t));
    memset(&robot_hurt_t, 0, sizeof(ext_robot_hurt_t));
    memset(&shoot_data_t, 0, sizeof(ext_shoot_data_t));
    memset(&bullet_remaining_t, 0, sizeof(ext_bullet_remaining_t));


    memset(&student_interactive_header_data_t, 0, sizeof(ext_student_interactive_header_data_t));



}

/**
  * @brief     pack data to bottom device
  * @param[in] cmd_id:  command id of data
  * @param[in] *p_data: pointer to the data to be sent
  * @param[in] len:     the data length
  */
void data_pack_handle(uint16_t cmd_id, uint8_t *p_data, uint16_t len)
{
  memset(referee_tx_buf, 0, REF_PROTOCOL_FRAME_MAX_SIZE);
  frame_header_t *p_header = (frame_header_t*)referee_tx_buf;
  
  p_header->SOF          = HEADER_SOF;
  p_header->data_length  = len;
  
  memcpy(&referee_tx_buf[REF_PROTOCOL_HEADER_SIZE], (uint8_t*)&cmd_id, REF_PROTOCOL_CMD_SIZE);
  append_CRC8_check_sum(referee_tx_buf, REF_PROTOCOL_HEADER_SIZE);
  
  memcpy(&referee_tx_buf[REF_PROTOCOL_HEADER_SIZE + REF_PROTOCOL_CMD_SIZE], p_data, len);
  append_CRC16_check_sum(referee_tx_buf, REF_HEADER_CRC_CMDID_LEN + len);
  
  HAL_UART_Transmit_DMA(&huart6,referee_tx_buf,REF_HEADER_CRC_CMDID_LEN+len);
}

void referee_data_solve(uint8_t *frame)
{
  uint16_t cmd_id = 0;
  uint8_t state_first_run = 1;
  uint8_t index = 0;

  memcpy(&referee_receive_header, frame, sizeof(frame_header_t));

  if(referee_receive_header.SOF == HEADER_SOF)
  {
    index += sizeof(frame_header_t);

    memcpy(&cmd_id, frame + index, sizeof(uint16_t));
    index += sizeof(uint16_t);

    switch (cmd_id)
    {
      case GAME_STATE_CMD_ID:
      {
          memcpy(&game_state, frame + index, sizeof(ext_game_state_t));
      }
      break;
      case GAME_RESULT_CMD_ID:
      {
          memcpy(&game_result, frame + index, sizeof(game_result));
      }
      break;
      case GAME_ROBOT_HP_CMD_ID:
      {
          memcpy(&game_robot_HP_t, frame + index, sizeof(ext_game_robot_HP_t));
      }
      break;


      case FIELD_EVENTS_CMD_ID:
      {
          memcpy(&field_event, frame + index, sizeof(field_event));
      }
      break;
      case SUPPLY_PROJECTILE_ACTION_CMD_ID:
      {
          memcpy(&supply_projectile_action_t, frame + index, sizeof(supply_projectile_action_t));
      }
      break;
      case SUPPLY_PROJECTILE_BOOKING_CMD_ID:
      {
//          memcpy(&supply_projectile_booking_t, frame + index, sizeof(supply_projectile_booking_t));
      }
      break;
      case REFEREE_WARNING_CMD_ID:
      {
          memcpy(&referee_warning_t, frame + index, sizeof(ext_referee_warning_t));
      }
      break;

      case ROBOT_STATE_CMD_ID:
      {  
          memcpy(&robot_state, frame + index, sizeof(robot_state));
          if(state_first_run)
          {
            switch(get_robot_id())
            {
              case RED_ENGINEER:
              {
                client_custom_character_t.sender_id = RED_ENGINEER;
                client_custom_character_t.receiver_id = RED_ENGINEER_CLIENT; 
              }
                break;
              case BLUE_ENGINEER:
              {
                client_custom_character_t.sender_id = BLUE_ENGINEER;
                client_custom_character_t.receiver_id = BLUE_ENGINEER_CLIENT;
              }
                break;
              default:
                break;
            }
            state_first_run = 0;
          }
      }
      break;
      case POWER_HEAT_DATA_CMD_ID:
      {
          memcpy(&power_heat_data_t, frame + index, sizeof(power_heat_data_t));
      }
      break;
      case ROBOT_POS_CMD_ID:
      {
          memcpy(&game_robot_pos_t, frame + index, sizeof(game_robot_pos_t));
      }
      break;
      case BUFF_MUSK_CMD_ID:
      {
          memcpy(&buff_t, frame + index, sizeof(buff_t));
      }
      break;
      case AERIAL_ROBOT_ENERGY_CMD_ID:
      {
          memcpy(&robot_energy_t, frame + index, sizeof(robot_energy_t));
      }
      break;
      case ROBOT_HURT_CMD_ID:
      {
          memcpy(&robot_hurt_t, frame + index, sizeof(robot_hurt_t));
      }
      break;
      case SHOOT_DATA_CMD_ID:
      {
          memcpy(&shoot_data_t, frame + index, sizeof(shoot_data_t));
      }
      break;
      case BULLET_REMAINING_CMD_ID:
      {
          memcpy(&bullet_remaining_t, frame + index, sizeof(ext_bullet_remaining_t));
      }
      break;
      case STUDENT_INTERACTIVE_DATA_CMD_ID:
      {
          memcpy(&student_interactive_header_data_t, frame + index, sizeof(student_interactive_header_data_t));
      }
      break;
			case CUSTOM_CONTROLLER_DATA_ID:
      {
          memcpy(&custom_controller_data_t, frame + index, sizeof(custom_controller_data_t));
      }
      break;
      default:
      {
          break;
      }
    }
  }
}

uint8_t get_robot_id(void)
{
  return robot_state.robot_id;
}

void draw_character(uint32_t opre_type)
{
	uint8_t i;
	client_custom_character_t.comment_id = DRAW_CHARACTER;
	client_custom_character_t.sender_id = RED_ENGINEER;
	client_custom_character_t.receiver_id =RED_ENGINEER_CLIENT;
	for(i=0;i<30;i++)
		client_custom_character_t.data[i] = ' ';
	client_custom_character_t.grapic_data_struct.graphic_name[0] = 'e';
	client_custom_character_t.grapic_data_struct.graphic_name[1] = 'n';
	client_custom_character_t.grapic_data_struct.graphic_name[2] = 'g';
	client_custom_character_t.grapic_data_struct.operate_tpye = opre_type;
  client_custom_character_t.grapic_data_struct.graphic_tpye = 7;   
  client_custom_character_t.grapic_data_struct.layer = 1;                   //0-9
  client_custom_character_t.grapic_data_struct.color = 5;										//粉色
  client_custom_character_t.grapic_data_struct.start_angle = 20;							 //字符大小
  client_custom_character_t.grapic_data_struct.end_angle = 30;		 						 //字符长度
  client_custom_character_t.grapic_data_struct.width = 2;											 //线条宽度
  client_custom_character_t.grapic_data_struct.start_x = 10;
  client_custom_character_t.grapic_data_struct.start_y = 900;
	client_custom_character_t.grapic_data_struct.radius = 0;
	client_custom_character_t.grapic_data_struct.end_x = 0;
	client_custom_character_t.grapic_data_struct.end_y = 0;
  
	i = 0;
	char_data[i++] = 'C';
	char_data[i++] = 'A';
	char_data[i++] = 'R';
	char_data[i++] = 'D';
	char_data[i++] = ':';
//	if(card == 0)
//	{
//		char_data[i++] = 'O';
//		char_data[i++] = 'F';
//		char_data[i++] = 'F';
//	}
//	else if(card == 1)
//	{
//		char_data[i++] = 'O';
//		char_data[i++] = 'N';
//	}
	for(;i<30;i++)
		char_data[i] = ' ';

  memcpy(&client_custom_character_t.data,char_data,sizeof(uint8_t[30]));
  
  data_pack_handle(STUDENT_INTERACTIVE_DATA_CMD_ID,(uint8_t *)&client_custom_character_t,CHAR_DATA_TOTAL_LEN);
}

void draw_front_sight(void)
{
	client_custom_graphic_double_t.comment_id = DRAW_DOUBLE_FIGURE;
	client_custom_graphic_double_t.sender_id = RED_ENGINEER;
	client_custom_graphic_double_t.receiver_id =RED_ENGINEER_CLIENT;
	
	client_custom_graphic_double_t.grapic_data_struct[0].graphic_name[0] = '1';
	client_custom_graphic_double_t.grapic_data_struct[0].graphic_name[1] = '1';
	client_custom_graphic_double_t.grapic_data_struct[0].graphic_name[2] = '1';
	client_custom_graphic_double_t.grapic_data_struct[0].operate_tpye = 1;
  client_custom_graphic_double_t.grapic_data_struct[0].graphic_tpye = 0;   
  client_custom_graphic_double_t.grapic_data_struct[0].layer = 8;                   //0-9
  client_custom_graphic_double_t.grapic_data_struct[0].color = 8;										//白色
  client_custom_graphic_double_t.grapic_data_struct[0].start_angle = 0;							 
  client_custom_graphic_double_t.grapic_data_struct[0].end_angle = 0;		 						
  client_custom_graphic_double_t.grapic_data_struct[0].width = 2;											 //线条宽度
  client_custom_graphic_double_t.grapic_data_struct[0].start_x = 840;
  client_custom_graphic_double_t.grapic_data_struct[0].start_y = 540;
	client_custom_graphic_double_t.grapic_data_struct[0].radius = 0;
	client_custom_graphic_double_t.grapic_data_struct[0].end_x = 1080;
	client_custom_graphic_double_t.grapic_data_struct[0].end_y = 540;
	
	client_custom_graphic_double_t.grapic_data_struct[1].graphic_name[0] = '2';
	client_custom_graphic_double_t.grapic_data_struct[1].graphic_name[1] = '2';
	client_custom_graphic_double_t.grapic_data_struct[1].graphic_name[2] = '2';
	client_custom_graphic_double_t.grapic_data_struct[1].operate_tpye = 1;
  client_custom_graphic_double_t.grapic_data_struct[1].graphic_tpye = 0;   
  client_custom_graphic_double_t.grapic_data_struct[1].layer = 9;                   //0-9
  client_custom_graphic_double_t.grapic_data_struct[1].color = 8;										//白色
  client_custom_graphic_double_t.grapic_data_struct[1].start_angle = 0;							 
  client_custom_graphic_double_t.grapic_data_struct[1].end_angle = 0;		 						
  client_custom_graphic_double_t.grapic_data_struct[1].width = 2;										//线条宽度
  client_custom_graphic_double_t.grapic_data_struct[1].start_x = 960;
  client_custom_graphic_double_t.grapic_data_struct[1].start_y = 0;
	client_custom_graphic_double_t.grapic_data_struct[1].radius = 0;
	client_custom_graphic_double_t.grapic_data_struct[1].end_x = 960;
	client_custom_graphic_double_t.grapic_data_struct[1].end_y = 1080;
	
	data_pack_handle(STUDENT_INTERACTIVE_DATA_CMD_ID,(uint8_t *)&client_custom_graphic_double_t,DOUBLE_FIGURE_TOTAL_LEN);
}

