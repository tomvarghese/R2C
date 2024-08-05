/*
 * R2c.hpp
 *
 *  Created on: Aug 3, 2024
 *      Author: TVarghese
 */

#ifndef R2C_HPP_
#define R2C_HPP_

#include R2C_Port.h

typedef uint32_t Time_t;

Time_t GetTime(void);


/**
 * @brief: Very basic run to completion scheduler that maintains a list of
 * tasks that are ready to run and calls their virtual Work() function when the
 * time that they are scheduled for happens.  Each task object maintains the
 * time that the scheduler uses to run them at.
 *
 * The tasks are scheduled using one of three functions:
 * 	StartIT - used by interrupt handles to run a task immediately
 * 	Start - used in a non-interrupt context run a task immediately
 * 	Start(time) - used to schedula a task to run at a specific time
 *
 * The static Run() function should be called from the foreground to run the
 * next available task.
 */
class R2cTask
{
	R2cTask *next;
	Time_t time;
	static volatile R2cTask *s_TaskList;

	virtual void Work(void) = 0; /* need to override this pure virtual function */

public:
	/**
	 * @brief : function that puts the object at the start of the Task list
	 * when running from an interrupt context ONLY. The task is put at the head
	 * of the queue, so it should run immediately.
	 */
	void StartIT(void) /* only function that should be called from an interrupt context */
	{
		this->time = GetTime();
		this->next = (R2cTask *) s_TaskList;
		s_TaskList = this;
	}

	/**
	 * @brief : function that puts the object at the start of the Task list from a
	 * non-interrupt context
	 */
	void Start(void)
	{
		Start(GetTime());
	}

	/**
	 * @brief : function that puts the object on Task list that is sorted by time.
	 * Called from non-interrupt context only.
	 */
	void Start(Time_t start_time)
	{
		time = start_time;

		__disable_irq();
		if (s_TaskList == nullptr || start_time < s_TaskList->time)
		{
			next = (R2cTask *) s_TaskList;
			s_TaskList = this;
			__enable_irq();
		}
		else
		{
			__enable_irq();
			for ( R2cTask *p = (R2cTask *) s_TaskList; p != nullptr; p = p->next)
			{
				if (p->next)
				{
					if (p->next->time > start_time)
					{
						next = p->next;
						p->next = this;
						break;
					}
				}
				else
				{
					next = p->next;
					p->next = this;
					break;
				}
			}
		}
	}

	static void Run(void);
};

#endif /* R2C_HPP_ */
