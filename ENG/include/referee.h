#ifndef REFEREE_H
#define REFEREE_H

#include "main.h"

#include "protocol.h"

typedef enum
{
  RED_HERO        = 1,
  RED_ENGINEER    = 2,
  RED_STANDARD_1  = 3,
  RED_STANDARD_2  = 4,
  RED_STANDARD_3  = 5,
  RED_AERIAL      = 6,
  RED_SENTRY      = 7,
  RED_RADAR       = 9,
  RED_OUTPOST     = 10,
  RED_BASE        = 11,
  BLUE_HERO       = 101,
  BLUE_ENGINEER   = 102,
  BLUE_STANDARD_1 = 103,
  BLUE_STANDARD_2 = 104,
  BLUE_STANDARD_3 = 105,
  BLUE_AERIAL     = 106,
  BLUE_SENTRY     = 107,
  BLUE_RADAR      = 109,
  BLUE_OUTPOST    = 110,
  BLUE_BASE       = 111,
} robot_id_t;

typedef enum
{
  RED_HERO_CILENT        = 0x0101,
  RED_ENGINEER_CLIENT    = 0x0102,
  RED_STANDARD_CLIENT_1  = 0x0103,
  RED_STANDARD_CLIENT_2  = 0x0104,
  RED_STANDARD_CLIENT_3  = 0x0105,
  RED_AERIAL_CLIENT      = 0x0106,
  BLUE_HERO_CILENT       = 0x0165,
  BLUE_ENGINEER_CLIENT   = 0x0166,
  BLUE_STANDARD_CLIENT_1 = 0x0167,
  BLUE_STANDARD_CLIENT_2 = 0x0168,
  BLUE_STANDARD_CLIENT_3 = 0x0169,
  BLUE_AERIAL_CLIENT     = 0x016A,  
} client_id_t;

typedef enum
{
  PROGRESS_UNSTART        = 0,
  PROGRESS_PREPARE        = 1,
  PROGRESS_SELFCHECK      = 2,
  PROGRESS_5sCOUNTDOWN    = 3,
  PROGRESS_BATTLE         = 4,
  PROGRESS_CALCULATING    = 5,
} game_progress_t;

typedef __packed struct //0001 
{
  uint8_t game_type : 4;
  uint8_t game_progress : 4;
  uint16_t stage_remain_time;
  uint64_t SyncTimeStamp;
} ext_game_state_t;

typedef __packed struct //0002
{
  uint8_t winner;
} ext_game_result_t;

typedef __packed struct //0003
{
  uint16_t red_1_robot_HP;  
  uint16_t red_2_robot_HP;  
  uint16_t red_3_robot_HP;
  uint16_t red_4_robot_HP;  
  uint16_t red_5_robot_HP;  
  uint16_t red_7_robot_HP;  

  uint16_t red_outpost_HP; 
  uint16_t red_base_HP;	  
  uint16_t blue_1_robot_HP; 
  uint16_t blue_2_robot_HP;
  uint16_t blue_3_robot_HP;
  uint16_t blue_4_robot_HP;
  uint16_t blue_5_robot_HP;
  uint16_t blue_7_robot_HP;

  uint16_t blue_outpost_HP;
  uint16_t blue_base_HP;
} ext_game_robot_HP_t;

typedef __packed struct //0101
{
  uint32_t event_type;
} ext_event_data_t;

typedef __packed struct //0x0102
{
  uint8_t reserved;
  uint8_t supply_robot_id;
  uint8_t supply_projectile_step;
  uint8_t supply_projectile_num;
} ext_supply_projectile_action_t;

typedef __packed struct //0104
{ 
  uint8_t level; 
  uint8_t offending_robot_id; 
  uint8_t count; 
}referee_warning_t; 


typedef __packed struct //0105
{ 
  uint8_t dart_remaining_time; 
  uint16_t dart_info; 
}dart_info_t;

typedef __packed struct //0201
{ 
  uint8_t robot_id; 
  uint8_t robot_level; 
  uint16_t current_HP;  
  uint16_t maximum_HP; 
  uint16_t shooter_barrel_cooling_value; 
  uint16_t shooter_barrel_heat_limit; 
  uint16_t chassis_power_limit;  
  uint8_t power_management_gimbal_output : 1; 
  uint8_t power_management_chassis_output : 1;  
  uint8_t power_management_shooter_output : 1; 
}robot_status_t; 


typedef __packed struct //0x0202
{
  uint16_t chassis_volt; 
  uint16_t chassis_current; 
  float chassis_power; 
  uint16_t chassis_power_buffer; 
  uint16_t shooter_id1_17mm_cooling_heat;
  uint16_t shooter_id2_17mm_cooling_heat;
  uint16_t shooter_id1_42mm_cooling_heat;
} ext_power_heat_data_t;

typedef __packed struct //0203
{ 
  float x; 
  float y; 
  float angle; 
}robot_pos_t; 

typedef __packed struct //0204
{ 
  uint8_t recovery_buff;  
  uint8_t cooling_buff;  
  uint8_t defence_buff;  
  uint8_t vulnerability_buff; 
  uint16_t attack_buff; 
}ext_buff_t; 

typedef __packed struct //0205
{ 
  uint8_t airforce_status; 
  uint8_t time_remain; 
}air_support_data_t; 

typedef __packed struct //0x0206
{
  uint8_t armor_id : 4;
  uint8_t hurt_type : 4;
} ext_robot_hurt_t;

typedef __packed struct //0x0207
{
  uint8_t bullet_type;

  uint8_t shooter_id;
  uint8_t bullet_freq;
  float bullet_speed;
} ext_shoot_data_t;

typedef __packed struct //0208
{
  uint16_t bullet_remaining_num_17mm;
  uint16_t bullet_remaining_num_42mm;
  uint16_t coin_remaining_num;
} ext_bullet_remaining_t;

typedef __packed struct //0209
{ 
	uint32_t rfid_status;
} ext_rfid_status_t;

typedef __packed struct //0301
{ 
  uint16_t data_cmd_id; 
  uint16_t sender_id; 
  uint16_t receiver_id; 
  uint8_t user_data[112]; 
}robot_interaction_data_t; 

typedef __packed struct
{
  uint8_t data[30];
} robot_interactive_data_t;

typedef __packed struct
{
  uint8_t operate_tpye; 
  uint8_t layer; 
} ext_client_custom_graphic_delete_t;

typedef __packed struct
{ 
  uint8_t graphic_name[3]; 
  uint32_t operate_tpye:3; 
  uint32_t graphic_tpye:3; 
  uint32_t layer:4; 
  uint32_t color:4; 
  uint32_t start_angle:9;
  uint32_t end_angle:9;
  uint32_t width:10; 
  uint32_t start_x:11; 
  uint32_t start_y:11; 
  uint32_t radius:10; 
  uint32_t end_x:11; 
  uint32_t end_y:11; 
} graphic_data_struct_t;

typedef __packed struct
{
  uint16_t comment_id;
  uint16_t sender_id;
  uint16_t receiver_id;
  graphic_data_struct_t grapic_data_struct;
} ext_client_custom_graphic_single_t;

typedef __packed struct
{
  uint16_t comment_id;
  uint16_t sender_id;
  uint16_t receiver_id;
  graphic_data_struct_t grapic_data_struct[2];
} ext_client_custom_graphic_double_t;

typedef __packed struct
{
	uint16_t comment_id;
  uint16_t sender_id;
  uint16_t receiver_id;
  graphic_data_struct_t grapic_data_struct[5];
} ext_client_custom_graphic_five_t;

typedef __packed struct
{
	uint16_t comment_id;
  uint16_t sender_id;
  uint16_t receiver_id;
  graphic_data_struct_t grapic_data_struct[7];
} ext_client_custom_graphic_seven_t;

typedef __packed struct
{
  uint16_t comment_id;
  uint16_t sender_id;
  uint16_t receiver_id;
  graphic_data_struct_t grapic_data_struct;
  uint8_t data[30];
} ext_client_custom_character_t;

typedef __packed struct //0x0303
{
  float target_position_x;
  float target_position_y;
  float target_position_z;
  uint8_t commd_keyboard;
  uint16_t target_robot_ID;
} ext_map_client_command_t;

typedef __packed struct //0x0305
{
uint16_t target_robot_ID;
float target_position_x;
float target_position_y;
} ext_client_map_command_t;

typedef __packed struct //0x0304
{
  int16_t mouse_x;
  int16_t mouse_y;
  int16_t mouse_z;
  int8_t left_button_down;
  int8_t right_button_down;
  uint16_t keyboard_value;
  uint16_t reserved;
} ext_robot_command_t;

//typedef __packed struct
//{
//	uint8_t key_1;
//	uint8_t key_2;
//	fp32 vx;
//	fp32 vy;
//	fp32 vz;
//	fp32 quaternion[4];
//} custom_controller_t;

typedef __packed struct
{
	int16_t encoder;
	uint16_t adc_value;
	uint8_t undefine[26];
} custom_controller_t;

extern custom_controller_t custom_controller_data_t;

extern void referee_data_solve(uint8_t *frame);
extern uint8_t get_robot_id(void);
void draw_character(uint32_t opre_type);
void draw_front_sight(void);
#endif
