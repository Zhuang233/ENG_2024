#ifndef __ROBOARM_H__
#define __ROBOARM_H__
#include "main.h"
#include "DJIMotoDriver.h"

#define LIFT (MotoState[4].angle_desired)
#define LIFT_READ (MotoState[4].angle)
#define LIFT_STD (-1273474)
#define LIFT_MIN_ANGLE_ALLOW_FIVE_JOINT_RESET LIFT_STD

#define QS (sync_data_to_c.data.qs_pos)
#define HY (sync_data_to_c.data.hy_pos)
#define PITCH (sync_data_to_c.data.theta1)
#define ROLL (sync_data_to_c.data.theta2)
#define YAW (sync_data_to_c.data.theta3)


# ifdef OLD_CAR
//pitch
#define ARM_ANGLE_MAX_1 45077
#define ARM_ANGLE_MIN_1 12650
//#define ARM_ANGLE_STD_1 28492
#define ARM_ANGLE_STD_1 ARM_ANGLE_MIN_1
//roll
#define ARM_ANGLE_MAX_2 59069
#define ARM_ANGLE_MIN_2 1
#define ARM_ANGLE_STD_2 25823


//yall
#define ARM_ANGLE_MAX_3 40000
#define ARM_ANGLE_MIN_3 10000
#define ARM_ANGLE_STD_3 24370

#define QS_ANGLE_MAX 780000
#define QS_ANGLE_MIN 0


#define HY_ANGLE_MAX 0
#define HY_ANGLE_MIN -390000

#define LIFT_MAX 0
#define LIFT_MIN -1950000

#define FLIP_MAX 0
#define FLIP_MIN -220000

#define SMALL_LIFT_MAX 410000
#define SMALL_LIFT_MIN 0

#define EXPAND_MAX 0
#define EXPAND_MIN -427240

void expand_init(void);
void flip_init(void);
void small_lift_init(void);

void Update_Expand_Pos(void);
void Update_Small_Lift_Pos(void);
void Update_Flip_Pos(void);
int32_t virtual_link(int32_t x);
#else

#define SMALL_QS (MotoState[5].angle_desired)
#define SMALL_QS_READ (MotoState[5].angle)

#define SMALL_LIFT (MotoState[6].angle_desired)
#define SMALL_LIFT_READ (MotoState[6].angle)

#define SMALL_YAW (MotoState[7].angle_desired)
#define SMALL_YAW_READ (MotoState[7].angle)

#define LIFT_CAMERA (MotoState[8].angle_desired)
#define LIFT_CAMERA_READ (MotoState[8].angle)

//pitch
#define ARM_ANGLE_MAX_1 32767
#define ARM_ANGLE_MIN_1 1
#define ARM_ANGLE_STD_1 1
#define PITCH_DOWN ARM_ANGLE_MAX_1
#define PITCH_UP ARM_ANGLE_MIN_1

//roll
#define ARM_ANGLE_MAX_2 62600
#define ARM_ANGLE_MIN_2 1
#define ARM_ANGLE_STD_2 33500
//#define ROLL_LEFT 
//#define ROLL_RIGHT 
#define ROLL_STD ARM_ANGLE_STD_2 

//yaw
#define ARM_ANGLE_MAX_3 32300
#define ARM_ANGLE_MIN_3 1
#define ARM_ANGLE_STD_3 16000
#define YAW_LEFT ARM_ANGLE_MAX_3
#define YAW_RIGHT ARM_ANGLE_MIN_3
#define YAW_STD ARM_ANGLE_STD_3

#define QS_ANGLE_MAX 780000
#define QS_ANGLE_MIN 0


#define HY_ANGLE_MAX 0
#define HY_ANGLE_MIN -389000

#define LIFT_MAX 0
#define LIFT_MIN -1950000

#define SMALL_YAW_MAX 465000
#define SMALL_YAW_MIN 0

#define SMALL_LIFT_MAX 452000
#define SMALL_LIFT_MIN 0

#define SMALL_QS_MAX 442000
#define SMALL_QS_MIN 0

#define LIFT_CAMERA_MAX 1788000
#define LIFT_CAMERA_MIN 0

void small_yaw_init(void);
void Update_Small_Yaw_Pos(void);

void small_lift_init(void);
void Update_Small_Lift_Pos(void);

void small_qs_init(void);
void Update_Small_Qs_Pos(void);

void lift_camera_init(void);
void Update_Lift_Camera_Pos(void);
#endif

void RoboArm_Pos_Init(void);
void RoboArm_RC_Ctrl(void);
void RoboArm_RC_Ctrl_Fixed_Point(void);
void SubArm_Ctrl(void);
void lift_init(void);
void Update_Lift_Pos(void);

#endif
