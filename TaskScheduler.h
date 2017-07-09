#ifndef _TASKSCHEDULER_H_
#define _TASKSCHEDULER_H_

/******************************************************************************
 * Author: Pratyush Kumar
 * Last Updated: July 9, 2017
 * Purpose: Implementation of a periodic task scheduler
 *****************************************************************************/

#include <string>
#include <cstring>
#include <unordered_map>
#include <utility>
#include <sqlite3.h>

struct taskType {
	std::string taskName;
	std::string metricName;
	std::string metricUnits;
};

class TaskScheduler {
	private:
		std::unordered_set<std::string, std::pair<std::string, bool>> activeTasks; // list of active tasks
		std::string sqliteDB;
		sqlite3 *db;
		bool verifySQLExec(int rc, char *zErrMsg); // error handling when executing SQL commands
		bool taskExists(std::string taskName); // check if the task has ever been initiated

	public:
		// opens a sqlite database _db
		TaskScheduler(std::string _db);

		// closes the sqlite database
		~TaskScheduler();

		// add a new task and set it's frequency
		void addTask(taskType task, int timeInterval, int timeToStart);

		// cancel a task
		void cancelTask(taskType task);

		// change the frequency of a task
		void rescheduleTask(taskType task, int timeInterval, int timeToStart);
};

#endif