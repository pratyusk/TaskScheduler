#ifndef _TASKSCHEDULER_H_
#define _TASKSCHEDULER_H_

/******************************************************************************
 * Author: Pratyush Kumar
 * Last Updated: July 9, 2017
 * Purpose: Implementation of a periodic task scheduler
 *****************************************************************************/

class TaskScheduler {
	private:


	public:
		TaskScheduler();
		~TaskScheduler();
		void addTask(int tid, );
		void cancelTask(int tid);
		void rescheduleTask();
};



#endif