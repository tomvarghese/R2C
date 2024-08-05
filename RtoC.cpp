/**
  ******************************************************************************
  * @file     R2C.cpp
  * @author   Tom Varghese
  * @version  V1.0
  * @date     03/08/2024 16:42:12
  * @brief    Simple Run to completion Scheduler
  ******************************************************************************
*/

/* Includes ------------------------------------------------------------------*/
#include "R2c.hpp"
/** Functions ----------------------------------------------------------------*/



R2cTask volatile *R2cTask::s_TaskList = nullptr;

static int idle_count = 0;
static uint32_t idle_time = 0;


static void Idle(void)
{
	idle_count++;
	/*
	 *  put into sleep mode someday
	 */
}


void R2cTask::Run(void)
{
	while(1)
	{
		if (s_TaskList)
		{
			if ( (int)(s_TaskList->time - GetTime()) <= 0 )
			{
				__disable_irq();
				R2cTask *t = (R2cTask *) s_TaskList;
				s_TaskList = s_TaskList->next;
				__enable_irq();
				t->Work();
			}
		}
		else
		{
			uint32_t idle_start = GetTime();
			Idle();
			idle_time += GetTime() - idle_start;
		}
	}
}
