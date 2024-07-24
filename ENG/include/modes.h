#ifndef __MODES_H
#define __MODES_H

typedef enum{
//	VIRTUAL_LINK,
	// 取金矿，全过程应均可手动微调，以及回退（左键确认，右键回退）
	NONE,
	FETCH_GOLD_INIT,   // 预备态ok
	FETCH_GOLD_INDEEP, // 深入ok
	FETCH_GOLD_INDEEP_UP,// 抬高 ok
	FETCH_GOLD_OUT, // 取出ok
	FETCH_GOLD_STORE_LEFT, // 存矿左ok
	FETCH_GOLD_STORE_RIGHT, // 存矿右 (歪)
//	FETCH_GOLD_INIT_LEFT, // 左预备态 8
//	FETCH_GOLD_INDEEP_LEFT, // 左深入 ok
//	FETCH_GOLD_INDEEP_UP_LEFT,// 左抬高 ok
//	FETCH_GOLD_OUT_LEFT, // 左取出 ok
	
	FETCH_SLIVER_INIT, // 上抬升 12
	FETCH_SLIVER_FLIP,		// 翻转吸 + 翻回存 + 翻回存2
	FETCH_SLIVWER_STORE_LEFT, //存
	SELECT_EXCANGE_MODE, //选择兑矿模式
	TAKE_GROUND_ORE_INIT,//取地矿初始姿态（可微调）
	TAKE_GROUND_ORE_TAKE_BACK,// 取地矿拿回来举着
//	FREE_ARM,// 自由机械臂,(最大限位，用于取地矿拨到合适姿态或者救援)
	PARA_FIND,  // 啥都不干，选手动参数用
	
	
	SINGEL_SLIVER_INIT,
	FETCH_SLIVER_AUTO,
	SINGEL_SLIVER_BACK,
	FETCH_GOLD_AUTO,
	DEBUG
}PoseMode;

extern PoseMode posemod;
extern PoseMode last_posemod;

#endif