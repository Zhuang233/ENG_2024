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
osThreadId defaultTaskHandle;
osThreadId TestHandle;
osThreadId MotoHandle;
osThreadId DataSyncAnCHandle;
osThreadId FlipHandle;
osThreadId CtrlLockHandle;
osThreadId AutoFetchHandle;
osThreadId TimeCtrlHandle;
osThreadId UIHandle;
osThreadId ChassisKeyHandle;
osThreadId ChassisMotoHandle;
osThreadId ModePoseHandle;
osThreadId RotationSlowHandle;
osThreadId LiftHandle;
osThreadId VirtualLinkHandle;
osThreadId DebugModeHandle;
osThreadId PosLimitHandle;
osThreadId OffsetHandle;

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */

/* USER CODE END FunctionPrototypes */

void StartDefaultTask(void const * argument);
void TestTask(void const * argument);
void MotoTask(void const * argument);
void DataSyncAnCTask(void const * argument);
void FlipTask(void const * argument);
void CtrlLockTask(void const * argument);
void AutoFetchTask(void const * argument);
void TimeCtrlTask(void const * argument);
void UITask(void const * argument);
void ChassisKeyTask(void const * argument);
void ChassisMotoTask(void const * argument);
void ModePoseTask(void const * argument);
void RotationSlowTask(void const * argument);
void LiftTask(void const * argument);
void VirtualLinkTask(void const * argument);
void DebugModeTask(void const * argument);
void PosLimitTask(void const * argument);
void OffsetTask(void const * argument);

void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/* GetIdleTaskMemory prototype (linked to static allocation support) */
void vApplicationGetIdleTaskMemory( StaticTask_t **ppxIdleTaskTCBBuffer, StackType_t **ppxIdleTaskStackBuffer, uint32_t *pulIdleTaskStackSize );

/* USER CODE BEGIN GET_IDLE_TASK_MEMORY */
static StaticTask_t xIdleTaskTCBBuffer;
static StackType_t xIdleStack[configMINIMAL_STACK_SIZE];

void vApplicationGetIdleTaskMemory( StaticTask_t **ppxIdleTaskTCBBuffer, StackType_t **ppxIdleTaskStackBuffer, uint32_t *pulIdleTaskStackSize )
{
  *ppxIdleTaskTCBBuffer = &xIdleTaskTCBBuffer;
  *ppxIdleTaskStackBuffer = &xIdleStack[0];
  *pulIdleTaskStackSize = configMINIMAL_STACK_SIZE;
  /* place for user code */
}
/* USER CODE END GET_IDLE_TASK_MEMORY */

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
  /* definition and creation of defaultTask */
  osThreadDef(defaultTask, StartDefaultTask, osPriorityNormal, 0, 128);
  defaultTaskHandle = osThreadCreate(osThread(defaultTask), NULL);

  /* definition and creation of Test */
  osThreadDef(Test, TestTask, osPriorityHigh, 0, 128);
  TestHandle = osThreadCreate(osThread(Test), NULL);

  /* definition and creation of Moto */
  osThreadDef(Moto, MotoTask, osPriorityHigh, 0, 128);
  MotoHandle = osThreadCreate(osThread(Moto), NULL);

  /* definition and creation of DataSyncAnC */
  osThreadDef(DataSyncAnC, DataSyncAnCTask, osPriorityIdle, 0, 128);
  DataSyncAnCHandle = osThreadCreate(osThread(DataSyncAnC), NULL);

  /* definition and creation of Flip */
  osThreadDef(Flip, FlipTask, osPriorityHigh, 0, 128);
  FlipHandle = osThreadCreate(osThread(Flip), NULL);

  /* definition and creation of CtrlLock */
  osThreadDef(CtrlLock, CtrlLockTask, osPriorityIdle, 0, 128);
  CtrlLockHandle = osThreadCreate(osThread(CtrlLock), NULL);

  /* definition and creation of AutoFetch */
  osThreadDef(AutoFetch, AutoFetchTask, osPriorityNormal, 0, 128);
  AutoFetchHandle = osThreadCreate(osThread(AutoFetch), NULL);

  /* definition and creation of TimeCtrl */
  osThreadDef(TimeCtrl, TimeCtrlTask, osPriorityIdle, 0, 128);
  TimeCtrlHandle = osThreadCreate(osThread(TimeCtrl), NULL);

  /* definition and creation of UI */
  osThreadDef(UI, UITask, osPriorityNormal, 0, 128);
  UIHandle = osThreadCreate(osThread(UI), NULL);

  /* definition and creation of ChassisKey */
  osThreadDef(ChassisKey, ChassisKeyTask, osPriorityNormal, 0, 128);
  ChassisKeyHandle = osThreadCreate(osThread(ChassisKey), NULL);

  /* definition and creation of ChassisMoto */
  osThreadDef(ChassisMoto, ChassisMotoTask, osPriorityNormal, 0, 128);
  ChassisMotoHandle = osThreadCreate(osThread(ChassisMoto), NULL);

  /* definition and creation of ModePose */
  osThreadDef(ModePose, ModePoseTask, osPriorityHigh, 0, 128);
  ModePoseHandle = osThreadCreate(osThread(ModePose), NULL);

  /* definition and creation of RotationSlow */
  osThreadDef(RotationSlow, RotationSlowTask, osPriorityNormal, 0, 128);
  RotationSlowHandle = osThreadCreate(osThread(RotationSlow), NULL);

  /* definition and creation of Lift */
  osThreadDef(Lift, LiftTask, osPriorityNormal, 0, 128);
  LiftHandle = osThreadCreate(osThread(Lift), NULL);

  /* definition and creation of VirtualLink */
  osThreadDef(VirtualLink, VirtualLinkTask, osPriorityNormal, 0, 128);
  VirtualLinkHandle = osThreadCreate(osThread(VirtualLink), NULL);

  /* definition and creation of DebugMode */
  osThreadDef(DebugMode, DebugModeTask, osPriorityNormal, 0, 128);
  DebugModeHandle = osThreadCreate(osThread(DebugMode), NULL);

  /* definition and creation of PosLimit */
  osThreadDef(PosLimit, PosLimitTask, osPriorityNormal, 0, 128);
  PosLimitHandle = osThreadCreate(osThread(PosLimit), NULL);

  /* definition and creation of Offset */
  osThreadDef(Offset, OffsetTask, osPriorityNormal, 0, 128);
  OffsetHandle = osThreadCreate(osThread(Offset), NULL);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

}

/* USER CODE BEGIN Header_StartDefaultTask */
/**
  * @brief  Function implementing the defaultTask thread.
  * @param  argument: Not used
  * @retval None
  */
/* USER CODE END Header_StartDefaultTask */
void StartDefaultTask(void const * argument)
{
  /* USER CODE BEGIN StartDefaultTask */
  /* Infinite loop */
  for(;;)
  {
    osDelay(1);
  }
  /* USER CODE END StartDefaultTask */
}

/* USER CODE BEGIN Header_TestTask */
/**
* @brief Function implementing the Test thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_TestTask */
__weak void TestTask(void const * argument)
{
  /* USER CODE BEGIN TestTask */
  /* Infinite loop */
  for(;;)
  {
    osDelay(1);
  }
  /* USER CODE END TestTask */
}

/* USER CODE BEGIN Header_MotoTask */
/**
* @brief Function implementing the Moto thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_MotoTask */
__weak void MotoTask(void const * argument)
{
  /* USER CODE BEGIN MotoTask */
  /* Infinite loop */
  for(;;)
  {
    osDelay(1);
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
__weak void DataSyncAnCTask(void const * argument)
{
  /* USER CODE BEGIN DataSyncAnCTask */
  /* Infinite loop */
  for(;;)
  {
    osDelay(1);
  }
  /* USER CODE END DataSyncAnCTask */
}

/* USER CODE BEGIN Header_FlipTask */
/**
* @brief Function implementing the Flip thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_FlipTask */
__weak void FlipTask(void const * argument)
{
  /* USER CODE BEGIN FlipTask */
  /* Infinite loop */
  for(;;)
  {
    osDelay(1);
  }
  /* USER CODE END FlipTask */
}

/* USER CODE BEGIN Header_CtrlLockTask */
/**
* @brief Function implementing the CtrlLock thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_CtrlLockTask */
__weak void CtrlLockTask(void const * argument)
{
  /* USER CODE BEGIN CtrlLockTask */
  /* Infinite loop */
  for(;;)
  {
    osDelay(1);
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
__weak void AutoFetchTask(void const * argument)
{
  /* USER CODE BEGIN AutoFetchTask */
  /* Infinite loop */
  for(;;)
  {
    osDelay(1);
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
__weak void TimeCtrlTask(void const * argument)
{
  /* USER CODE BEGIN TimeCtrlTask */
  /* Infinite loop */
  for(;;)
  {
    osDelay(1);
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
__weak void UITask(void const * argument)
{
  /* USER CODE BEGIN UITask */
  /* Infinite loop */
  for(;;)
  {
    osDelay(1);
  }
  /* USER CODE END UITask */
}

/* USER CODE BEGIN Header_ChassisKeyTask */
/**
* @brief Function implementing the ChassisKey thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_ChassisKeyTask */
__weak void ChassisKeyTask(void const * argument)
{
  /* USER CODE BEGIN ChassisKeyTask */
  /* Infinite loop */
  for(;;)
  {
    osDelay(1);
  }
  /* USER CODE END ChassisKeyTask */
}

/* USER CODE BEGIN Header_ChassisMotoTask */
/**
* @brief Function implementing the ChassisMoto thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_ChassisMotoTask */
__weak void ChassisMotoTask(void const * argument)
{
  /* USER CODE BEGIN ChassisMotoTask */
  /* Infinite loop */
  for(;;)
  {
    osDelay(1);
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
__weak void ModePoseTask(void const * argument)
{
  /* USER CODE BEGIN ModePoseTask */
  /* Infinite loop */
  for(;;)
  {
    osDelay(1);
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
__weak void RotationSlowTask(void const * argument)
{
  /* USER CODE BEGIN RotationSlowTask */
  /* Infinite loop */
  for(;;)
  {
    osDelay(1);
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
__weak void LiftTask(void const * argument)
{
  /* USER CODE BEGIN LiftTask */
  /* Infinite loop */
  for(;;)
  {
    osDelay(1);
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
__weak void VirtualLinkTask(void const * argument)
{
  /* USER CODE BEGIN VirtualLinkTask */
  /* Infinite loop */
  for(;;)
  {
    osDelay(1);
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
__weak void DebugModeTask(void const * argument)
{
  /* USER CODE BEGIN DebugModeTask */
  /* Infinite loop */
  for(;;)
  {
    osDelay(1);
  }
  /* USER CODE END DebugModeTask */
}

/* USER CODE BEGIN Header_PosLimitTask */
/**
* @brief Function implementing the PosLimit thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_PosLimitTask */
__weak void PosLimitTask(void const * argument)
{
  /* USER CODE BEGIN PosLimitTask */
  /* Infinite loop */
  for(;;)
  {
    osDelay(1);
  }
  /* USER CODE END PosLimitTask */
}

/* USER CODE BEGIN Header_OffsetTask */
/**
* @brief Function implementing the Offset thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_OffsetTask */
__weak void OffsetTask(void const * argument)
{
  /* USER CODE BEGIN OffsetTask */
  /* Infinite loop */
  for(;;)
  {
    osDelay(1);
  }
  /* USER CODE END OffsetTask */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */

/* USER CODE END Application */
