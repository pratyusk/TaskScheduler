#include "TaskScheduler.h"

void TaskScheduler::stringToLower(std::string &inputString) {
	std::transform(inputString.begin(), inputString.end(), inputString.begin(), ::tolower);
}

bool TaskScheduler::verifySQLExec(int rc, char *zErrMsg) {
	if (rc != SQLITE_OK) {
		std::cerr << "SQL error: " << zErrMsg << std::endl;
		sqlite3_free(zErrMsg);
	} else {
		std::cout << "Table created successfully" << std::endl;
	}
}

bool TaskScheduler::taskExists(std::string taskName) {
	return (activeTasks.find(taskName) != activeTasks.end());
}

TaskScheduler::TaskScheduler(std::string _db) : sqliteDB(_db) {
	int rc = sqlite3_open(sqliteDB.c_str(), &db);
	char *zErrMsg = 0;
   	if (rc) {
		std::cerr << "Can't open database: " << sqlite3_errmsg(db) << std::endl;
		return;
   	} else {
      	std::cout << "Opened database successfully" << std::endl;
      	std::string sql = "CREATE TABLE SAMPLES (TASK_NAME TEXT," \
						  "METRIC_NAME TEXT," \
						  "METRIC_VALUE REAL DEFAULT 0," \
						  "METRIC_UNITS TEXT," \
						  "LAST_UPDATED TEXT DEFAULT(datetime('now', 'localtime'))," \
						  "PRIMARY KEY (TASK_NAME, METRIC_NAME));";
		rc = sqlite3_exec(sqliteDB.c_str(), sql, NULL, 0, &zErrMsg); // execute the SQL command
		verifySQLExec(rc, zErrMsg);
   	}
}

TaskScheduler::~TaskScheduler() {
	sqlite3_close(db);
}

void TaskScheduler::addTask(taskType task, int timeInterval, int timeToStart) {
	stringToLower(taskName);
	// if the task was never initiated; or if it was initiated, it was not for this particular metric, then create a new row
	if (!taskExists(task.taskName)
		|| (taskExists(task.taskName) && activeTasks[task.taskName].first != task.metricName)) {
		activeTasks[task.taskName] = std::make_pair<std::string, bool>(task.metricName, true); // mark task as active
		std::string sql = "INSERT INTO SAMPLES (TASK_NAME, METRIC_NAME, METRIC_UNITS) " \
						  "VALUES (" + task.taskName + ", " + task.metricName + ", " + \
						  task.metricUnits + ");";
		char *zErrMsg = 0;
		int rc = sqlite3_exec(db, sql, NULL, 0, &zErrMsg); // execute the SQL command
		verifySQLExec(rc, zErrMsg);
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

void cancelTask(taskType task) {
	stringToLower(taskName);
	if (!taskExists(task.taskName)) {
		std::cerr << "Trying to cancel a non-existent task" << std::endl;
	} else if (!activeTasks[task.taskName].second) {
		std::cerr << "Trying to cancel an inactive task" << std::endl;
	} else {
		if (activeTasks[task.taskName].second) {
			activeTasks[task.taskName].second = false;
		}
	}
}

void rescheduleTask(taskType task, int timeInterval, int timeToStart) {
	stringToLower(taskName);
	if (!taskExists(task.taskName)) {
		std::cerr << "Trying to reschedule a non-existent task" << std::endl;
	} else {
		sql = "UPDATE SAMPLES set " +  = 25000.00 where ID=1; "
	}
}

