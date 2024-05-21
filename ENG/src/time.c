#include "time.h"

void TimeInit(TimeTD *time){
	time->dt = 0.01f;
	time->timer_cnt_last = 0;
	time->timer_cnt_now = 0;
	time->timer_cnt_total = 0;
}


void GetDt(TimeTD *time, uint32_t time_unit)
{
	uint32_t d_cnt = 0;
	
	//time->timer_cnt_now = __HAL_TIM_GET_COUNTER(&PID_TIMER_HANDEL);
	time->timer_cnt_now = TIM2->CNT;
	if(time->timer_cnt_now >= time->timer_cnt_last)
	{
		d_cnt = time->timer_cnt_now - time->timer_cnt_last;
	}
	else
	{
		d_cnt = time->timer_cnt_now + (0xffffffff - time->timer_cnt_last);
	}
	
	if(time_unit == SECOND)
		time->dt = (float)d_cnt/1000000.0f;
	else if(time_unit == MILLISECOND)
		time->dt = (float)d_cnt/1000.0f;
	
	time->timer_cnt_last=time->timer_cnt_now;
	time->timer_cnt_total+=d_cnt;
}