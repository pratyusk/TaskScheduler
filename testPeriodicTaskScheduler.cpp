#include <iostream>
#include <string>
#include <algorithm>
#include <unistd.h>
#include <vector>
#include "TaskScheduler.h"
#include "ConnectTCPServer.h"
#include "CalculatePhysicalMemory.h"

/******************************************************************************
 * Author: Pratyush Kumar
 * Last Updated: July 9, 2017
 * Purpose: Generic test case for periodic task scheduler
 * (open sampleTesh.sh for more details)
 *****************************************************************************/

template <typename T>
void checkStreamState(bool state, T &val) {
	if (state) return;
	while (!(std::cin >> val)) {
		std::cerr << "Please try again!" << std::endl;
	}
}

void stringToLower(std::string &inputString) {
	std::transform(inputString.begin(), inputString.end(), inputString.begin(), ::tolower);
}

bool checkCommand(char command) {
	if (command != 'A' && command != 'C' && command != 'R' && command != 'E' && command != 'S') {
		std::cerr << "Please enter a valid command!" << std::endl;
		return false;
	}
	return true;
}

bool checkTask(std::string taskName, std::vector<std::string> &listOfPrograms) {
	stringToLower(taskName);
	if (find(listOfPrograms.begin(), listOfPrograms.end(), taskName) == listOfPrograms.end()) {
		std::cerr << "Please enter a valid task name! (Available: ConnectTCPServer, CalculatePhysicalMemory)" << std::endl;
		return false;
	}
	return true;
}

bool checkMetric(std::string metricName, std::string taskName, std::unordered_map<std::string, std::string> &metricMap) {
	stringToLower(metricName);
	stringToLower(taskName);
	if (metricMap[taskName] != metricName) {
		std::cerr << "Please enter a valid metric name for the given task name! \n(Available: " \
		"ConnectTime for ConnectTCPServer, PhysicalMemory for CalculatePhysicalMemory)" << std::endl;
		return false;
	}
	return true;
}

//
int main(int argc, char **argv) {
	std::vector<std::string> listOfPrograms;
	std::string program1 = "connecttcpserver";
	std::string program2 = "calculatephysicalmemory";
	listOfPrograms.push_back(program1);
	listOfPrograms.push_back(program2);
	std::unordered_map<std::string, std::string> metricMap;
	metricMap[program1] = "connecttime";
	metricMap[program2] = "physicalmemory";
	TaskScheduler scheduler("sampleDB.db");
	std::cout << "*** Tasks available : CalculatePhysicalMemory, ConnectTCPServer ***" << std::endl;
	std::cout << "*** Commands ***" << std::endl;
	std::cout << "\tAdd Task : 'A <task_name> <metric_name> <metric_units> <time_interval> [<addr>] [<port>]'" << std::endl;
	std::cout << "\t\tNOTE : For ConnectTCPServer, provide address and port number (whitespace separated); " \
				 "time interval in seconds" << std::endl;
	std::cout << "\tCancel Task : 'C <task_name> <metric_name>'" << std::endl;
	std::cout << "\tReschedule Task : 'R <task_name> <metric_name> <new_time_interval>'" << std::endl;
	std::cout << "\t\tNOTE : All times are in seconds" << std::endl;
	std::cout << "\tSleep : 'S <wait_time>(in seconds)'\n" << std::endl;
	std::cout << "\tExit test case : 'E'\n" << std::endl;
	while (1) {
		std::cout << "Please enter a command:" << std::endl;
		char command;
		checkStreamState(std::cin >> command, command);
		command = toupper(command);
		if (!checkCommand(command)) continue;
		if (command == 'E') break;
		if (command == 'S') {
			int waitTime;
			checkStreamState(std::cin >> waitTime, waitTime);
			std::cout << "waiting for " << waitTime << " seconds\n" << std::endl;
			sleep(waitTime);
			continue;
		}
		std::string taskName, metricName;
		checkStreamState(std::cin >> taskName, taskName);
		while (!checkTask(taskName, listOfPrograms)) {
			checkStreamState(std::cin >> taskName, taskName);
		}
		stringToLower(taskName);
		checkStreamState(std::cin >> metricName, metricName);
		while (!checkMetric(metricName, taskName, metricMap)) {
			checkStreamState(std::cin >> metricName, metricName);
		}
		stringToLower(metricName);
		if (command == 'A') {
			std::string metricUnits;
			checkStreamState(std::cin >> metricUnits, metricUnits);
			int timeInterval;
			checkStreamState(std::cin >> timeInterval, timeInterval);
			taskType task;
			task.taskName = taskName;
			task.metricName = metricName;
			task.metricUnits = metricUnits;
			if (taskName == listOfPrograms[1]) {
				std::function<int()> func = CalculatePhysicalMemory;
				scheduler.addTask(task, timeInterval, func);
			} else if (taskName == listOfPrograms[0]) {
				std::string addr, port;
				checkStreamState(std::cin >> addr, addr);
				checkStreamState(std::cin >> port, port);
				std::function<double(const char *, const char *)> func = ConnectTCPServer;
				scheduler.addTask(task, timeInterval, func, addr.c_str(), port.c_str());
			}
		} else if (command	== 'R') {
			int newTimeInterval;
			checkStreamState(std::cin >> newTimeInterval, newTimeInterval);
			scheduler.rescheduleTask(taskName, metricName, newTimeInterval);
		} else if (command == 'C') {
			scheduler.cancelTask(taskName, metricName);
		}
		std::cout << "command completed: " << command << " " << taskName << " " << metricName << "\n" << std::endl;
	}
	return 0;
}