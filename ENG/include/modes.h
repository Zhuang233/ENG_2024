#ifndef __MODES_H
#define __MODES_H

typedef enum{
//	VIRTUAL_LINK,
	// ȡ���ȫ����Ӧ�����ֶ�΢�����Լ����ˣ����ȷ�ϣ��Ҽ����ˣ�
	NONE,
	FETCH_GOLD_INIT,   // Ԥ��̬ok
	FETCH_GOLD_INDEEP, // ����ok
	FETCH_GOLD_INDEEP_UP,// ̧�� ok
	FETCH_GOLD_OUT, // ȡ��ok
	FETCH_GOLD_STORE_LEFT, // �����ok
	FETCH_GOLD_STORE_RIGHT, // ����� (��)
//	FETCH_GOLD_INIT_LEFT, // ��Ԥ��̬ 8
//	FETCH_GOLD_INDEEP_LEFT, // ������ ok
//	FETCH_GOLD_INDEEP_UP_LEFT,// ��̧�� ok
//	FETCH_GOLD_OUT_LEFT, // ��ȡ�� ok
	
	FETCH_SLIVER_INIT, // ��̧�� 12
	FETCH_SLIVER_FLIP,		// ��ת�� + ���ش� + ���ش�2
	FETCH_SLIVWER_STORE_LEFT, //��
	SELECT_EXCANGE_MODE, //ѡ��ҿ�ģʽ
	TAKE_GROUND_ORE_INIT,//ȡ�ؿ��ʼ��̬����΢����
	TAKE_GROUND_ORE_TAKE_BACK,// ȡ�ؿ��û�������
//	FREE_ARM,// ���ɻ�е��,(�����λ������ȡ�ؿ󲦵�������̬���߾�Ԯ)
	PARA_FIND,  // ɶ�����ɣ�ѡ�ֶ�������
	
	
	SINGEL_SLIVER_INIT,
	FETCH_SLIVER_AUTO,
	SINGEL_SLIVER_BACK,
	FETCH_GOLD_AUTO,
	DEBUG
}PoseMode;

extern PoseMode posemod;
extern PoseMode last_posemod;

#endif