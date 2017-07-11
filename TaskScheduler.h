#ifndef _TASKSCHEDULER_H_
#define _TASKSCHEDULER_H_

/******************************************************************************
 * Author: Pratyush Kumar
 * Last Updated: July 9, 2017
 * Purpose: Implementation of a periodic task scheduler
 *****************************************************************************/

#include <iostream>
#include <string>
#include <cstring>
#include <unordered_map>
#include <utility>
#include <thread>
#include <functional>
#include <algorithm>
#include <sqlite3.h>

struct taskType {
	std::string taskName;
	std::string metricName;
	std::string metricUnits;
};

class TaskScheduler {
	private:
		// list of active tasks, maps taskName to a metricName and active status
		std::unordered_map<std::string, std::pair<std::string, bool>> activeTasks;
		std::string sqliteDB; // name of current database
		sqlite3 *db;
		bool verifySQLExec(int rc, char *zErrMsg); // error handling when executing SQL commands
		bool taskExists(std::string taskName, std::string metricName); // check if the task has ever been initiated
		void stringToLower(std::string &inputString); // convert string to lower case

		template <typename T, typename... U>
		void runTask(taskType &task, T func, U... args); // run the task given by func

	public:
		// opens a sqlite database _db
		TaskScheduler(std::string _db);

		// closes the sqlite database
		~TaskScheduler();

		// add a new task and set it's frequency
		template <typename T, typename... U>
		void addTask(taskType task, int timeInterval, T func, U... args);

		// cancel a task
		void cancelTask(std::string taskName, std::string metricName);

		// change the frequency of a task
		void rescheduleTask(std::string taskName, std::string metricName, int timeInterval);
};

#endif