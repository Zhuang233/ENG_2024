/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * File Name          : freertos.c
  * Description        : Code for freertos applications
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2024 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN Variables */

/* USER CODE END Variables */
/* Definitions for defaultTask */
osThreadId_t defaultTaskHandle;
const osThreadAttr_t defaultTask_attributes = {
  .name = "defaultTask",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityLow,
};
/* Definitions for Moto */
osThreadId_t MotoHandle;
const osThreadAttr_t Moto_attributes = {
  .name = "Moto",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityHigh,
};
/* Definitions for DataSyncAnC */
osThreadId_t DataSyncAnCHandle;
const osThreadAttr_t DataSyncAnC_attributes = {
  .name = "DataSyncAnC",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};
/* Definitions for CtrlLock */
osThreadId_t CtrlLockHandle;
const osThreadAttr_t CtrlLock_attributes = {
  .name = "CtrlLock",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};
/* Definitions for AutoFetch */
osThreadId_t AutoFetchHandle;
const osThreadAttr_t AutoFetch_attributes = {
  .name = "AutoFetch",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};
/* Definitions for TimeCtrl */
osThreadId_t TimeCtrlHandle;
const osThreadAttr_t TimeCtrl_attributes = {
  .name = "TimeCtrl",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};
/* Definitions for UI */
osThreadId_t UIHandle;
const osThreadAttr_t UI_attributes = {
  .name = "UI",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};
/* Definitions for ChassisMoto */
osThreadId_t ChassisMotoHandle;
const osThreadAttr_t ChassisMoto_attributes = {
  .name = "ChassisMoto",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};
/* Definitions for ModePose */
osThreadId_t ModePoseHandle;
const osThreadAttr_t ModePose_attributes = {
  .name = "ModePose",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityHigh,
};
/* Definitions for RotationSlow */
osThreadId_t RotationSlowHandle;
const osThreadAttr_t RotationSlow_attributes = {
  .name = "RotationSlow",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};
/* Definitions for Lift */
osThreadId_t LiftHandle;
const osThreadAttr_t Lift_attributes = {
  .name = "Lift",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};
/* Definitions for VirtualLink */
osThreadId_t VirtualLinkHandle;
const osThreadAttr_t VirtualLink_attributes = {
  .name = "VirtualLink",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};
/* Definitions for DebugMode */
osThreadId_t DebugModeHandle;
const osThreadAttr_t DebugMode_attributes = {
  .name = "DebugMode",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};
/* Definitions for Offset */
osThreadId_t OffsetHandle;
const osThreadAttr_t Offset_attributes = {
  .name = "Offset",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};
/* Definitions for canRecv */
osThreadId_t canRecvHandle;
const osThreadAttr_t canRecv_attributes = {
  .name = "canRecv",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityHigh,
};
/* Definitions for SecondArm */
osThreadId_t SecondArmHandle;
const osThreadAttr_t SecondArm_attributes = {
  .name = "SecondArm",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityHigh,
};
/* Definitions for ModeManage */
osThreadId_t ModeManageHandle;
const osThreadAttr_t ModeManage_attributes = {
  .name = "ModeManage",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityHigh,
};
/* Definitions for ModeExec */
osThreadId_t ModeExecHandle;
const osThreadAttr_t ModeExec_attributes = {
  .name = "ModeExec",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityHigh,
};

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */

/* USER CODE END FunctionPrototypes */

void StartDefaultTask(void *argument);
void MotoTask(void *argument);
void DataSyncAnCTask(void *argument);
void CtrlLockTask(void *argument);
void AutoFetchTask(void *argument);
void TimeCtrlTask(void *argument);
void UITask(void *argument);
void ChassisMotoTask(void *argument);
void ModePoseTask(void *argument);
void RotationSlowTask(void *argument);
void LiftTask(void *argument);
void VirtualLinkTask(void *argument);
void DebugModeTask(void *argument);
void OffsetTask(void *argument);
void canRecvTask(void *argument);
void SecondArmTask(void *argument);
void ModeManageTask(void *argument);
void ModeExecTask(void *argument);

void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/**
  * @brief  FreeRTOS initialization
  * @param  None
  * @retval None
  */
void MX_FREERTOS_Init(void) {
  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* USER CODE BEGIN RTOS_MUTEX */
  /* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */
  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* creation of defaultTask */
  defaultTaskHandle = osThreadNew(StartDefaultTask, NULL, &defaultTask_attributes);

  /* creation of Moto */
  MotoHandle = osThreadNew(MotoTask, NULL, &Moto_attributes);

  /* creation of DataSyncAnC */
  DataSyncAnCHandle = osThreadNew(DataSyncAnCTask, NULL, &DataSyncAnC_attributes);

  /* creation of CtrlLock */
  CtrlLockHandle = osThreadNew(CtrlLockTask, NULL, &CtrlLock_attributes);

  /* creation of AutoFetch */
  AutoFetchHandle = osThreadNew(AutoFetchTask, NULL, &AutoFetch_attributes);

  /* creation of TimeCtrl */
  TimeCtrlHandle = osThreadNew(TimeCtrlTask, NULL, &TimeCtrl_attributes);

  /* creation of UI */
  UIHandle = osThreadNew(UITask, NULL, &UI_attributes);

  /* creation of ChassisMoto */
  ChassisMotoHandle = osThreadNew(ChassisMotoTask, NULL, &ChassisMoto_attributes);

  /* creation of ModePose */
  ModePoseHandle = osThreadNew(ModePoseTask, NULL, &ModePose_attributes);

  /* creation of RotationSlow */
  RotationSlowHandle = osThreadNew(RotationSlowTask, NULL, &RotationSlow_attributes);

  /* creation of Lift */
  LiftHandle = osThreadNew(LiftTask, NULL, &Lift_attributes);

  /* creation of VirtualLink */
  VirtualLinkHandle = osThreadNew(VirtualLinkTask, NULL, &VirtualLink_attributes);

  /* creation of DebugMode */
  DebugModeHandle = osThreadNew(DebugModeTask, NULL, &DebugMode_attributes);

  /* creation of Offset */
  OffsetHandle = osThreadNew(OffsetTask, NULL, &Offset_attributes);

  /* creation of canRecv */
  canRecvHandle = osThreadNew(canRecvTask, NULL, &canRecv_attributes);

  /* creation of SecondArm */
  SecondArmHandle = osThreadNew(SecondArmTask, NULL, &SecondArm_attributes);

  /* creation of ModeManage */
  ModeManageHandle = osThreadNew(ModeManageTask, NULL, &ModeManage_attributes);

  /* creation of ModeExec */
  ModeExecHandle = osThreadNew(ModeExecTask, NULL, &ModeExec_attributes);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

  /* USER CODE BEGIN RTOS_EVENTS */
  /* add events, ... */
  /* USER CODE END RTOS_EVENTS */

}

/* USER CODE BEGIN Header_StartDefaultTask */
/**
  * @brief  Function implementing the defaultTask thread.
  * @param  argument: Not used
  * @retval None
  */
/* USER CODE END Header_StartDefaultTask */
void StartDefaultTask(void *argument)
{
  /* USER CODE BEGIN StartDefaultTask */
  /* Infinite loop */
  for(;;)
  {
    osDelay(1);
  }
  /* USER CODE END StartDefaultTask */
}

/* USER CODE BEGIN Header_MotoTask */
/**
* @brief Function implementing the Moto thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_MotoTask */
__weak void MotoTask(void *argument)
{
  /* USER CODE BEGIN MotoTask */
  /* Infinite loop */
  for(;;)
  {
    osDelay(1000);
  }
  /* USER CODE END MotoTask */
}

/* USER CODE BEGIN Header_DataSyncAnCTask */
/**
* @brief Function implementing the DataSyncAnC thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_DataSyncAnCTask */
__weak void DataSyncAnCTask(void *argument)
{
  /* USER CODE BEGIN DataSyncAnCTask */
  /* Infinite loop */
  for(;;)
  {
    osDelay(1000);
  }
  /* USER CODE END DataSyncAnCTask */
}

/* USER CODE BEGIN Header_CtrlLockTask */
/**
* @brief Function implementing the CtrlLock thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_CtrlLockTask */
__weak void CtrlLockTask(void *argument)
{
  /* USER CODE BEGIN CtrlLockTask */
  /* Infinite loop */
  for(;;)
  {
    osDelay(1000);
  }
  /* USER CODE END CtrlLockTask */
}

/* USER CODE BEGIN Header_AutoFetchTask */
/**
* @brief Function implementing the AutoFetch thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_AutoFetchTask */
__weak void AutoFetchTask(void *argument)
{
  /* USER CODE BEGIN AutoFetchTask */
  /* Infinite loop */
  for(;;)
  {
    osDelay(1000);
  }
  /* USER CODE END AutoFetchTask */
}

/* USER CODE BEGIN Header_TimeCtrlTask */
/**
* @brief Function implementing the TimeCtrl thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_TimeCtrlTask */
__weak void TimeCtrlTask(void *argument)
{
  /* USER CODE BEGIN TimeCtrlTask */
  /* Infinite loop */
  for(;;)
  {
    osDelay(1000);
  }
  /* USER CODE END TimeCtrlTask */
}

/* USER CODE BEGIN Header_UITask */
/**
* @brief Function implementing the UI thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_UITask */
__weak void UITask(void *argument)
{
  /* USER CODE BEGIN UITask */
  /* Infinite loop */
  for(;;)
  {
    osDelay(1000);
  }
  /* USER CODE END UITask */
}

/* USER CODE BEGIN Header_ChassisMotoTask */
/**
* @brief Function implementing the ChassisMoto thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_ChassisMotoTask */
__weak void ChassisMotoTask(void *argument)
{
  /* USER CODE BEGIN ChassisMotoTask */
  /* Infinite loop */
  for(;;)
  {
    osDelay(1000);
  }
  /* USER CODE END ChassisMotoTask */
}

/* USER CODE BEGIN Header_ModePoseTask */
/**
* @brief Function implementing the ModePose thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_ModePoseTask */
__weak void ModePoseTask(void *argument)
{
  /* USER CODE BEGIN ModePoseTask */
  /* Infinite loop */
  for(;;)
  {
    osDelay(1000);
  }
  /* USER CODE END ModePoseTask */
}

/* USER CODE BEGIN Header_RotationSlowTask */
/**
* @brief Function implementing the RotationSlow thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_RotationSlowTask */
__weak void RotationSlowTask(void *argument)
{
  /* USER CODE BEGIN RotationSlowTask */
  /* Infinite loop */
  for(;;)
  {
    osDelay(1000);
  }
  /* USER CODE END RotationSlowTask */
}

/* USER CODE BEGIN Header_LiftTask */
/**
* @brief Function implementing the Lift thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_LiftTask */
__weak void LiftTask(void *argument)
{
  /* USER CODE BEGIN LiftTask */
  /* Infinite loop */
  for(;;)
  {
    osDelay(1000);
  }
  /* USER CODE END LiftTask */
}

/* USER CODE BEGIN Header_VirtualLinkTask */
/**
* @brief Function implementing the VirtualLink thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_VirtualLinkTask */
__weak void VirtualLinkTask(void *argument)
{
  /* USER CODE BEGIN VirtualLinkTask */
  /* Infinite loop */
  for(;;)
  {
    osDelay(1000);
  }
  /* USER CODE END VirtualLinkTask */
}

/* USER CODE BEGIN Header_DebugModeTask */
/**
* @brief Function implementing the DebugMode thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_DebugModeTask */
__weak void DebugModeTask(void *argument)
{
  /* USER CODE BEGIN DebugModeTask */
  /* Infinite loop */
  for(;;)
  {
    osDelay(1000);
  }
  /* USER CODE END DebugModeTask */
}

/* USER CODE BEGIN Header_OffsetTask */
/**
* @brief Function implementing the Offset thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_OffsetTask */
__weak void OffsetTask(void *argument)
{
  /* USER CODE BEGIN OffsetTask */
  /* Infinite loop */
  for(;;)
  {
    osDelay(1000);
  }
  /* USER CODE END OffsetTask */
}

/* USER CODE BEGIN Header_canRecvTask */
/**
* @brief Function implementing the canRecv thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_canRecvTask */
__weak void canRecvTask(void *argument)
{
  /* USER CODE BEGIN canRecvTask */
  /* Infinite loop */
  for(;;)
  {
    osDelay(1000);
  }
  /* USER CODE END canRecvTask */
}

/* USER CODE BEGIN Header_SecondArmTask */
/**
* @brief Function implementing the SecondArm thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_SecondArmTask */
__weak void SecondArmTask(void *argument)
{
  /* USER CODE BEGIN SecondArmTask */
  /* Infinite loop */
  for(;;)
  {
    osDelay(1);
  }
  /* USER CODE END SecondArmTask */
}

/* USER CODE BEGIN Header_ModeManageTask */
/**
* @brief Function implementing the ModeManage thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_ModeManageTask */
__weak void ModeManageTask(void *argument)
{
  /* USER CODE BEGIN ModeManageTask */
  /* Infinite loop */
  for(;;)
  {
    osDelay(1);
  }
  /* USER CODE END ModeManageTask */
}

/* USER CODE BEGIN Header_ModeExecTask */
/**
* @brief Function implementing the ModeExec thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_ModeExecTask */
__weak void ModeExecTask(void *argument)
{
  /* USER CODE BEGIN ModeExecTask */
  /* Infinite loop */
  for(;;)
  {
    osDelay(1);
  }
  /* USER CODE END ModeExecTask */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */

/* USER CODE END Application */

