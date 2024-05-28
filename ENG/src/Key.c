#include "Key.h"
#include "RcDriver.h"

KEYS Keys;
MOUSE Mouse;

void Single_Key_Init(KEY_MSG* msg,uint16_t key_code)
{
	msg->code = key_code;
	msg->press_flag  = 0;
	msg->toggle_flag = 0;
}

void Single_Mouse_Init(MOUSE_MSG* msg)
{
	msg->press_flag  = 0;
	msg->toggle_flag = 0;
}

void Mouth_Init(void)		//����ʼ��
{
	uint8_t i=0;
	MOUSE_MSG *pMsg = &Mouse.LEFT;
	for(i=0;i<2;i++)
		Single_Mouse_Init(pMsg+i);
}

void Key_Init(void)		//16��������ʼ��
{
	uint8_t i=0;
	KEY_MSG *pMsg = &Keys.KEY_W;
	for(i=0;i<16;i++)
		Single_Key_Init(pMsg+i, 1<<i);
}

/// @brief	������ ? 1(��һ��) : 0
bool Key_Check_Press(KEY_MSG* msg)	
{
	// �ϴ�û���£���ΰ���
	if((RC_CtrlData.key.info & msg->code) && !msg->press_flag)
	{
		msg->press_flag = 1;
		return 1;
	}
	// �ϴΰ��£����û����
	if(!(RC_CtrlData.key.info & msg->code) && msg->press_flag)
		msg->press_flag = 0;
	return 0;
}


bool Mouse_Check_Press(MOUSE_MSG* msg)	
{
	// �ϴ�û���£���ΰ���
	if((RC_CtrlData.mouse.press_l) && !msg->press_flag)
	{
		msg->press_flag = 1;
		return 1;
	}
	// �ϴΰ��£����û����
	if(!(RC_CtrlData.mouse.press_l) && msg->press_flag)
		msg->press_flag = 0;
	return 0;
}

/// @brief	������ ? 1 : 0
bool Key_Check_Hold(KEY_MSG* msg)
{
	if((RC_CtrlData.key.info & msg->code) && !msg->press_flag)
		msg->press_flag = 1;
	if(!(RC_CtrlData.key.info & msg->code) && msg->press_flag)
		msg->press_flag = 0;
	return msg->press_flag;
}


bool Mouse_Check_Hold(MOUSE_MSG* msg)
{
	if(RC_CtrlData.mouse.press_l  && !msg->press_flag)
		msg->press_flag = 1;
	if(!RC_CtrlData.mouse.press_l && msg->press_flag)
		msg->press_flag = 0;
	return msg->press_flag;
}


/// @brief	������ ? ��־λ^=1(��һ��)
bool Key_Check_Toggle(KEY_MSG* msg)
{
	if(Key_Check_Press(msg))
		msg->toggle_flag ^= 1;
	return msg->toggle_flag;
}

