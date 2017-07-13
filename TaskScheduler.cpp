#include "TaskScheduler.h"

int TaskScheduler::callback(void *data, int argc, char **argv, char **azColName) {
	double *dataPtr = static_cast<double *>(data);
	double metricValue = *dataPtr;
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
	average += (metricValue / numTimesRun);
	minimum = std::min(minimum, metricValue);
	maximum = std::max(maximum, metricValue);
	std::string sql = "UPDATE SAMPLES SET METRIC_VALUE = " + std::to_string(metricValue) + ", " \
					  "NUM_TIMES_RUN = " + std::to_string(numTimesRun) + ", AVERAGE = " + std::to_string(average) + ", " \
					  "MINIMUM = " + std::to_string(minimum) + ", MAXIMUM = " + std::to_string(maximum) + ", " \
					  "LAST_UPDATED = datetime('now', 'localtime') WHERE " \
					  "TASK_NAME = '" + taskName + "' AND METRIC_NAME = '" + metricName + "';";
	char *zErrMsg = 0;
	int rc = sqlite3_exec(db, sql.c_str(), NULL, 0, &zErrMsg);
	if (rc != SQLITE_OK) {
		std::cerr << "SQL error: " << zErrMsg << std::endl;
		sqlite3_free(zErrMsg);
		*dataPtr = -1;
	} else {
		std::string timeInterval(argv[2]);
		*dataPtr = std::stod(timeInterval); // update data to TIME_INTERVAL
	}
	std::cout << "Task: " << taskName << " metric: " << metricName << std::endl;
	std::cout << "Value: " << metricValue << " " << argv[7] << std::endl;
	std::cout << "NUM_TIMES_RUN: " << numTimesRun << std::endl;
	std::cout << "AVERAGE: " << average << std::endl;
	std::cout << "MINIMUM: " << minimum << std::endl;
	std::cout << "MAXIMUM: " << maximum << std::endl;
	return 0;
}

bool TaskScheduler::verifySQLExec(int rc, char *zErrMsg) {
	if (rc != SQLITE_OK) {
		std::cerr << "SQL error: " << zErrMsg << std::endl;
		sqlite3_free(zErrMsg);
		return false;
	}
	return true;
}

void TaskScheduler::stringToLower(std::string &inputString) {
	std::transform(inputString.begin(), inputString.end(), inputString.begin(), ::tolower);
}

bool TaskScheduler::taskExists(std::string taskName, std::string metricName) {
	if (activeTasks.find(taskName) != activeTasks.end()) {
		if (activeTasks[taskName].first == metricName) return true;
	}
	return false;
}

TaskScheduler::TaskScheduler(std::string _db) : sqliteDB(_db) {
	int rc = sqlite3_open(sqliteDB.c_str(), &db);
	char *zErrMsg = 0;
   	if (rc) {
		std::cerr << "Can't open database: " << sqlite3_errmsg(db) << std::endl;
		return;
   	} else {
      	std::cout << "Opened database successfully" << std::endl;
      	std::string sql = "DROP TABLE IF EXISTS SAMPLES;";
      	rc = sqlite3_exec(db, sql.c_str(), NULL, 0, &zErrMsg); // execute the SQL command
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
		rc = sqlite3_exec(db, sql.c_str(), NULL, 0, &zErrMsg); // execute the SQL command
		verifySQLExec(rc, zErrMsg);
   	}
}

TaskScheduler::~TaskScheduler() {
	sqlite3_close(db);
}

void TaskScheduler::cancelTask(std::string taskName, std::string metricName) {
	stringToLower(taskName);
	stringToLower(metricName);
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
}

void TaskScheduler::rescheduleTask(std::string taskName, std::string metricName, int timeInterval) {
	stringToLower(taskName);
	stringToLower(metricName);
	if (!taskExists(taskName, metricName)) {
		std::cerr << "Trying to reschedule a non-existent task" << std::endl;
	} else {
		char *zErrMsg = 0;
		std::string sql = "UPDATE SAMPLES SET TIME_INTERVAL = " + std::to_string(timeInterval) + " WHERE TASK_NAME = " \
			  "'" + taskName + "' AND METRIC_NAME = '" + metricName + "';";
		int rc = sqlite3_exec(db, sql.c_str(), NULL, 0, &zErrMsg); // execute the SQL command
		verifySQLExec(rc, zErrMsg);
	}
}