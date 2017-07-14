#ifndef _TASKSCHEDULER_H_
#define _TASKSCHEDULER_H_

/******************************************************************************
 * Author: Pratyush Kumar
 * Last Updated: July 13, 2017
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
#include <tuple>
#include <mutex>
#include <sqlite3.h>
#include <unistd.h>

extern std::mutex cout_lock; // to prevent jumbled stream to terminal
extern std::mutex globals_lock; // to protect global shared resourses

// input data to sql command callback function
struct callbackData {
	double metricValue;
	sqlite3 *db;
};

// function called during sql execution
static int callback(void *data, int argc, char **argv, char **azColName) {
	callbackData *dataPtr = static_cast<callbackData *>(data);
	double metricValue = dataPtr->metricValue;
	if (metricValue < 0) return 0;
	std::string taskName(argv[0]);
	std::string metricName(argv[1]);
	std::string numTimesRunString(argv[3]);
	double numTimesRun = std::stod(numTimesRunString);
	std::string averageString(argv[4]);
	double average = std::stod(averageString);
	std::string minimunString(argv[5]);
	double minimum = std::stod(minimunString);
	std::string maximumString(argv[6]);
	double maximum = std::stod(maximumString);
	numTimesRun++;
	average -= average / numTimesRun;
	average += metricValue / numTimesRun;
	if (numTimesRun == 1) minimum = metricValue;
	else minimum = std::min(minimum, metricValue);
	maximum = std::max(maximum, metricValue);
	std::string sql = "UPDATE SAMPLES SET METRIC_VALUE = " + std::to_string(metricValue) + ", " \
					  "NUM_TIMES_RUN = " + std::to_string(numTimesRun) + ", AVERAGE = " + std::to_string(average) + ", " \
					  "MINIMUM = " + std::to_string(minimum) + ", MAXIMUM = " + std::to_string(maximum) + ", " \
					  "LAST_UPDATED = datetime('now', 'localtime') WHERE " \
					  "TASK_NAME = '" + taskName + "' AND METRIC_NAME = '" + metricName + "';";
	char *zErrMsg = 0;
	int rc = sqlite3_exec(dataPtr->db, sql.c_str(), NULL, 0, &zErrMsg);
	if (rc != SQLITE_OK) {
		std::cerr << "SQL error: " << zErrMsg << std::endl;
		sqlite3_free(zErrMsg);
		dataPtr->metricValue = -1;
	} else {
		std::string timeInterval(argv[2]);
		dataPtr->metricValue = std::stod(timeInterval); // update data to TIME_INTERVAL
	}
	std::cout << "Task: " << taskName << " metric: " << metricName << std::endl;
	std::cout << "Value: " << metricValue << " " << argv[7] << std::endl;
	std::cout << "NUM_TIMES_RUN: " << numTimesRun << std::endl;
	std::cout << "AVERAGE: " << average << std::endl;
	std::cout << "MINIMUM: " << minimum << std::endl;
	std::cout << "MAXIMUM: " << maximum << std::endl;
	std::cout << "TIME_INTERVAL: " << argv[2] << std::endl;
	std::cout << std::endl;
	return 0;
}

// wrapper for task details
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
		sqlite3 *db; // sqlite database pointer

		bool verifySQLExec(int rc, char *zErrMsg); // error handling when executing SQL commands
		bool taskExists(std::string taskName, std::string metricName); // check if the task has ever been initiated
		void stringToLower(std::string &inputString); // convert string to lower case

		template <typename T, typename... U>
		void runTask(taskType task, T func, U... args) { // run task func
			char *zErrMsg = 0;
			int rc;
			callbackData data;
			data.metricValue = 0;
			globals_lock.lock();
			data.db = db;
			globals_lock.unlock();
			std::string sql = "SELECT TASK_NAME, METRIC_NAME, TIME_INTERVAL, NUM_TIMES_RUN, AVERAGE, MINIMUM, MAXIMUM, METRIC_UNITS " \
				  "FROM SAMPLES WHERE TASK_NAME = '" + task.taskName + \
				  "' AND METRIC_NAME = '" + task.metricName + "';";
			while(1) {
				data.metricValue = func(args...);
				globals_lock.lock();
				rc = sqlite3_exec(db, sql.c_str(), callback, (void *)&data, &zErrMsg);
				globals_lock.unlock();
				if (!verifySQLExec(rc, zErrMsg)) break; // invalid SQL
				if (data.metricValue < 0) break; // some error during callback
				globals_lock.lock();
				if (!activeTasks[task.taskName].second) { // task cancelled
					globals_lock.unlock();
					break;
				}
				globals_lock.unlock();
				sleep(static_cast<int>(data.metricValue)); // updated data is sleep time in seconds
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
			globals_lock.lock();
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
				// there is a bug in gcc compiler (4.8.5) which prevents creation of lambda
				// functions with parameter packs. so I have manually extracted the parameters
				// for this project. It should, however, for any number of arguments with
				// gcc 4.9.0 or higher or clang++ compiler.
				// auto it = [=]() {
				// 	runTask(task, func, args...);
				// };

				std::tuple<U...> argsTuple = std::tuple<U...>(args...);
				auto lambdaFunc = [=]() {
					runTask(task, func, std::get<0>(argsTuple), std::get<1>(argsTuple));
				};
				std::thread t1(lambdaFunc);
				t1.detach();
			} else {
				if (activeTasks[task.taskName].first == task.metricName
				    && !activeTasks[task.taskName].second) { // this task exists but is inactive
					activeTasks[task.taskName].second = true;
				} else { // task is already running
					std::cerr << "Trying to add a task that's already running. " \
								 "Please call rescheduleTask instead!" << std::endl;
				}
			}
			globals_lock.unlock();
		}

		// cancel a task
		void cancelTask(std::string taskName, std::string metricName);

		// change the frequency of a task
		void rescheduleTask(std::string taskName, std::string metricName, int timeInterval);
};

#endif