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

// callback function for select statements
static int callback(void *data, int argc, char **argv, char **azColName) {
	double *data1 = static_cast<double *>(data);
	*data1 = 10;
	for (int i = 0; i < argc; i++) {
		printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
	}
	printf("\n");
	return 0;
}

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
		void runTask(taskType &task, T &func, U... args) { // run task func
			char *zErrMsg = 0;
			std::string sql;
			int rc;
			double data = 0;
			sql = "SELECT TIME_INTERVAL, NUM_TIMES_RUN, AVERAGE, MINIMUM, MAXIMUM " \
				  "FROM SAMPLES WHERE TASK_NAME = '" + task.taskName + \
				  "' AND METRIC_NAME = '" + task.metricName + "';";
			while(1) {
				data = func(args...);
				rc = sqlite3_exec(db, sql.c_str(), callback, (void *)&data, &zErrMsg);
				if (!verifySQLExec(rc, zErrMsg)) break;
				if (data < 0) break;
				break;
			}
		}

	public:
		// opens a sqlite database _db
		TaskScheduler(std::string _db);

		// closes the sqlite database
		~TaskScheduler();

		// add a new task and set it's frequency
		template <typename T, typename... U>
		void addTask(taskType task, int timeInterval, T func, U... args) {
			stringToLower(task.taskName);
			stringToLower(task.metricName);
			// if the task was never initiated then create a new row
			if (!taskExists(task.taskName, task.metricName)) {
				activeTasks[task.taskName] = std::make_pair(task.metricName, true); // mark task as active
				std::string sql = "INSERT INTO SAMPLES (TASK_NAME, METRIC_NAME, METRIC_UNITS, TIME_INTERVAL) " \
								  "VALUES ('" + task.taskName + "', '" + task.metricName + "', '" \
								  + task.metricUnits + "', " + std::to_string(timeInterval) + ");";
				char *zErrMsg = 0;
				int rc = sqlite3_exec(db, sql.c_str(), NULL, 0, &zErrMsg); // execute the SQL command
				if (!verifySQLExec(rc, zErrMsg)) {
					return;
				}
				// std::thread t1(runTask<, task, func, args...);
				// t1.join();
				runTask(task, func, args...);
			} else {
				if (activeTasks[task.taskName].first == task.metricName
				    && !activeTasks[task.taskName].second) { // this task exists but is inactive
					activeTasks[task.taskName].second = true;
				} else { // task is already running
					std::cerr << "Trying to add a task that's already running. " \
								 "Please call rescheduleTask instead!" << std::endl;
				}
			}
		}

		// cancel a task
		void cancelTask(std::string taskName, std::string metricName);

		// change the frequency of a task
		void rescheduleTask(std::string taskName, std::string metricName, int timeInterval);
};

#endif