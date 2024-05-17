#include "LKMotoDriver.h"

uint8_t rx_data_lk[8]; //FIFO接收缓存区
LKMotoStateTD LKMotoState[8];


void LKUpdateMotoState(LKMotoStateTD* state);

void LKSetSpeed(LKMotorIdTD moto_id, uint32_t speed){
	CAN_TxHeaderTypeDef canTxHeader;
	uint32_t send_mail_box;
	uint8_t TX_Data[8];

	canTxHeader.StdId = moto_id;
	canTxHeader.ExtId = 0x00;
	canTxHeader.RTR = CAN_RTR_DATA;
	canTxHeader.IDE = CAN_ID_STD;
	canTxHeader.DLC = 8;
	
	TX_Data[0] = 0xA2;
	TX_Data[1] = 0x00;
	TX_Data[2] = 0x00;
	TX_Data[3] = 0x00;
	TX_Data[4] = *(uint8_t*)(&speed);
	TX_Data[5] = *((uint8_t*)(&speed)+1);
	TX_Data[6] = *((uint8_t*)(&speed)+2);
	TX_Data[7] = *((uint8_t*)(&speed)+3);
	
	/* send can command */
	HAL_CAN_AddTxMessage(&LK_MOTO_CAN_HANDEL, &canTxHeader, TX_Data, &send_mail_box);
}


void LKSaveMotoMsg(CAN_HandleTypeDef *hcan, uint32_t RxFifo){
	CAN_RxHeaderTypeDef Rx_Msg;

  HAL_CAN_GetRxMessage(hcan, RxFifo, &Rx_Msg, rx_data_lk);					// 接收can1 fifo0邮箱的数据帧。不接收会导致fifo邮箱一直爆满，无限进入接收中断，卡死所有任务。

  switch (Rx_Msg.StdId)			//can1 motor message decode
  {
		case LK_Motor1_ID:
		case LK_Motor2_ID:
		case LK_Motor3_ID:
		case LK_Motor4_ID:
		case LK_Motor5_ID:
		case LK_Motor6_ID:
		case LK_Motor7_ID:
		case LK_Motor8_ID:
			{
				static uint8_t i = 0;
				//get motor id
				i = Rx_Msg.StdId - LK_Motor1_ID; // 组内编号
				LKUpdateMotoState(&LKMotoState[i]);
				break;
			}
    default:
				break;		
  }

}

void LKUpdateMotoState(LKMotoStateTD* state){
	


}
