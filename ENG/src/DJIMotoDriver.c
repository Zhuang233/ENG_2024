#include "DJIMotoDriver.h"
//#include "trace.h"

void UpdateMotoState(MotoStateTD* MotoState);
void UpdateMotoAngle(MotoStateTD* MotoState);
void SaveMotoCurrent(CAN_HandleTypeDef *hcan, uint32_t RxFifo);

uint8_t rx_data[8]; //FIFO接收缓存区
MotoStateTD MotoState[16];

void MotoStateInit(MotoStateTD* motostate)
{
	motostate->angle = 0;
	motostate->angle_actual = 0;
	motostate->angle_desired = 0;
	motostate->angle_last = 0;
	motostate->first_run = true;
	motostate->given_current = 0;
	motostate->original_position = 0;
	motostate->real_current = 0;
	motostate->speed_actual = 0;
	motostate->speed_desired = 0;
	motostate->speed_last = 0;
	motostate->temperature = 0;
	motostate->turns = 0;
}



// 发送电机电流值
// 参数：can句柄，电机组别，电机1 2 3 4电流
void SetMotoCurrent(CAN_HandleTypeDef* hcan, MotoGroupe group, int16_t C1, int16_t C2, int16_t C3, int16_t C4)
{
  uint8_t TX_Data[8];
  uint32_t send_mail_box;
	CAN_TxHeaderTypeDef Tx_Msg;
	
	// 两组电机的标识符
	if(group == Ahead){
		Tx_Msg.StdId=0x200;
	}
	else{
		Tx_Msg.StdId=0x1FF;
	}
	
	Tx_Msg.IDE=CAN_ID_STD;		  // 不使用扩展标识符
	Tx_Msg.RTR=CAN_RTR_DATA;		// 消息类型为数据帧，
	Tx_Msg.DLC=8; 							//一帧8字节
  
  TX_Data[0] = C1 >> 8;
	TX_Data[1] = C1;
	TX_Data[2] = C2 >> 8;
	TX_Data[3] = C2;
	TX_Data[4] = C3 >> 8;
	TX_Data[5] = C3;
	TX_Data[6] = C4 >> 8;
	TX_Data[7] = C4;
  
  HAL_CAN_AddTxMessage(hcan, &Tx_Msg, TX_Data, &send_mail_box);    //将数据储存进邮箱FIFOx
}

// 接收电机反馈消息
void SaveMotoMsg(CAN_HandleTypeDef *hcan, uint32_t RxFifo){
	CAN_RxHeaderTypeDef Rx_Msg;

  HAL_CAN_GetRxMessage(hcan, RxFifo, &Rx_Msg, rx_data);					// 接收can1 fifo0邮箱的数据帧。不接收会导致fifo邮箱一直爆满，无限进入接收中断，卡死所有任务。

  switch (Rx_Msg.StdId)			//can1 motor message decode
  {
		case CAN_Motor1_ID:
		case CAN_Motor2_ID:
		case CAN_Motor3_ID:
		case CAN_Motor4_ID:
		case CAN_Motor5_ID:
		case CAN_Motor6_ID:
		case CAN_Motor7_ID:
		case CAN_Motor8_ID:
			{
				static uint8_t i = 0;
				//get motor id
				i = Rx_Msg.StdId - CAN_Motor1_ID;
				if(hcan == &hcan2) i += 5;
				UpdateMotoState(&MotoState[i]);
				break;
			}
    default:
				break;		
  }
}


// 更新电机状态
void UpdateMotoState(MotoStateTD* MotoState){
	MotoState->angle_last = MotoState->angle_actual;
	MotoState->speed_last = MotoState->speed_actual;
	
	MotoState->angle_actual = (uint16_t)(rx_data[0] << 8 | rx_data[1]);
	MotoState->speed_actual = (int16_t)(rx_data[2] << 8 | rx_data[3]); 
//	trace_speed(MotoState->speed_actual);
	
	MotoState->real_current = (int16_t)(rx_data[4] << 8 | rx_data[5]);
//	trace_current(MotoState->real_current);
	
	MotoState->temperature = rx_data[6];
  
  if(MotoState->first_run == true)                  //first_run为true时，记下初始位置
  {
    MotoState->angle = 0;
    MotoState->turns = 0;
    MotoState->angle_last = 0;
    MotoState->original_position = MotoState->angle_actual;
    MotoState->first_run = false;
  }
  else
    UpdateMotoAngle(MotoState);
}


//calcuelate angle of motor
int16_t d_angle;
void UpdateMotoAngle(MotoStateTD* MotoState)
{
	d_angle = MotoState->angle_actual - MotoState->angle_last;
	if(d_angle <= -4096) MotoState->turns += 1;
	else if(d_angle >= 4096) MotoState->turns -= 1;
	MotoState->angle = MotoState->turns * 8192 + MotoState->angle_actual - MotoState->original_position;
}

