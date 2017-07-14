#include "TaskScheduler.h"

// locks
std::mutex cout_lock;
std::mutex globals_lock;

// verify the sql command being executed
bool TaskScheduler::verifySQLExec(int rc, char *zErrMsg) {
	if (rc != SQLITE_OK) {
		cout_lock.lock();
		std::cerr << "SQL error: " << zErrMsg << std::endl;
		cout_lock.unlock();
		sqlite3_free(zErrMsg);
		return false;
	}
	return true;
}

// helper function to make commands case-insensitive
void TaskScheduler::stringToLower(std::string &inputString) {
	std::transform(inputString.begin(), inputString.end(), inputString.begin(), ::tolower);
}

// check whether the task is currently running
bool TaskScheduler::taskExists(std::string taskName, std::string metricName) {
	if (activeTasks.find(taskName) != activeTasks.end()) {
		if (activeTasks[taskName].first == metricName) return true;
	}
	return false;
}

// constructor - initialize the database
TaskScheduler::TaskScheduler(std::string _db) : sqliteDB(_db) {
	int rc = sqlite3_open(sqliteDB.c_str(), &db);
	char *zErrMsg = 0;
   	if (rc) {
		std::cerr << "Can't open database: " << sqlite3_errmsg(db) << std::endl;
		return;
   	} else {
      	std::cout << "Opened database successfully" << std::endl;
      	std::string sql = "DROP TABLE IF EXISTS SAMPLES;";
      	globals_lock.lock();
      	rc = sqlite3_exec(db, sql.c_str(), NULL, 0, &zErrMsg); // execute the SQL command
      	globals_lock.unlock();
		if (!verifySQLExec(rc, zErrMsg)) return;
      	sql = "CREATE TABLE SAMPLES (TASK_NAME TEXT NOT NULL, " \
			  "METRIC_NAME TEXT NOT NULL, " \
			  "METRIC_VALUE REAL DEFAULT 0, " \
			  "METRIC_UNITS TEXT, " \
			  "TIME_INTERVAL INTEGER DEFAULT 0, " \
			  "NUM_TIMES_RUN INTEGER DEFAULT 0, " \
			  "AVERAGE REAL DEFAULT 0, " \
			  "MINIMUM REAL DEFAULT 0, " \
			  "MAXIMUM REAL DEFAULT 0, " \
			  "LAST_UPDATED TEXT DEFAULT(datetime('now', 'localtime')), " \
			  "PRIMARY KEY (TASK_NAME, METRIC_NAME));"; // TIME_INTERVAL in seconds
		globals_lock.lock();
		rc = sqlite3_exec(db, sql.c_str(), NULL, 0, &zErrMsg); // execute the SQL command
		globals_lock.unlock();
		verifySQLExec(rc, zErrMsg);
   	}
}

// destructor - close the database
TaskScheduler::~TaskScheduler() {
	sqlite3_close(db);
}

void TaskScheduler::cancelTask(std::string taskName, std::string metricName) {
	stringToLower(taskName);
	stringToLower(metricName);
	globals_lock.lock();
	if (!taskExists(taskName, metricName)) {
		std::cerr << "Trying to cancel a non-existent task" << std::endl;
	} else if (!activeTasks[taskName].second) {
		std::cerr << "Trying to cancel an inactive task" << std::endl;
	} else {
		if (activeTasks[taskName].second) {
			std::cout << "Task cancelled : " << taskName << std::endl;
			activeTasks[taskName].second = false;
		}
	}
	globals_lock.unlock();
}

void TaskScheduler::rescheduleTask(std::string taskName, std::string metricName, int newTimeInterval) {
	stringToLower(taskName);
	stringToLower(metricName);
	globals_lock.lock();
	if (!taskExists(taskName, metricName)) {
		std::cerr << "Trying to reschedule a non-existent task" << std::endl;
	} else {
		char *zErrMsg = 0;
		std::string sql = "UPDATE SAMPLES SET TIME_INTERVAL = " + std::to_string(newTimeInterval) + " WHERE " \
						  "TASK_NAME = '" + taskName + "' AND METRIC_NAME = '" + metricName + "';";
		int rc = sqlite3_exec(db, sql.c_str(), NULL, 0, &zErrMsg); // execute the SQL command
		verifySQLExec(rc, zErrMsg);
	}
	globals_lock.unlock();
}