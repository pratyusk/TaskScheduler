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

//
int main(int argc, char **argv) {
	std::vector<std::string> listOfPrograms;
	std::string program1 = "connecttcpserver";
	std::string program2 = "calculatephysicalmemory";
	listOfPrograms.push_back(program1);
	listOfPrograms.push_back(program2);
	TaskScheduler scheduler("sampleDB.db");
	std::cout << "*** Tasks available : CalculatePhysicalMemory, ConnectTCPServer ***" << std::endl;
	std::cout << "*** Commands ***" << std::endl;
	std::cout << "\tAdd Task : 'A <task_name> [<addr>] [<port>] <time_interval> <time_to_start_from_now>'" << std::endl;
	std::cout << "\t\tNOTE : For ConnectTCPServer, provide address and port number (whitespace separated)" << std::endl;
	std::cout << "\tCancel Task : 'C <task_name>'" << std::endl;
	std::cout << "\tReschedule Task : 'R <task_name> <new_time_interval> <time_to_start_from_now>'" << std::endl;
	std::cout << "\t\tNOTE : All times are in seconds" << std::endl;
	std::cout << "\tSleep : 'S <wait_time>(in seconds)'\n" << std::endl;
	std::cout << "\tExit test case : 'E'\n" << std::endl;
	while (1) {
		std::cout << "Please enter a command:" << std::endl;
		char command;
		checkStreamState(std::cin >> command, command);
		command = toupper(command);
		// std::cout << "command is : " << command << std::endl;
		if (!checkCommand(command)) continue;
		if (command == 'E') break;
		if (command == 'S') {
			int waitTime;
			checkStreamState(std::cin >> waitTime, waitTime);
			sleep(waitTime);
			continue;
		}
		std::string taskName;
		checkStreamState(std::cin >> taskName, taskName);
		while (!checkTask(taskName, listOfPrograms)) {
			checkStreamState(std::cin >> taskName, taskName);
		}
		std::transform(taskName.begin(), taskName.end(), taskName.begin(), ::tolower);
		if (command == 'A') {
			int timeInterval;
			checkStreamState(std::cin >> timeInterval, timeInterval);
			std::string metricName, metricUnits;
			checkStreamState(std::cin >> metricName, metricName);
			checkStreamState(std::cin >> metricUnits, metricUnits);
			taskType task;
			task.taskName = taskName;
			task.metricName = metricName;
			task.metricUnits = metricUnits;
			stringToLower(taskName);
			if (taskName == listOfPrograms[1]) {
				std::function<int()> func = CalculatePhysicalMemory;
				scheduler.addTask<decltype(func)>(task, timeInterval, func);
			} else if (taskName == listOfPrograms[0]) {
				std::string addr, port;
				checkStreamState(std::cin >> addr, addr);
				checkStreamState(std::cin >> port, port);
				std::function<double(const char *, const char *)> func = ConnectTCPServer;
				scheduler.addTask<decltype(func), const char *, const char *>(task, timeInterval, func, addr.c_str(), port.c_str());
			}

		}
		if (command	== 'R') {
			int newTimeInterval;
			checkStreamState(std::cin >> newTimeInterval, newTimeInterval);
		}
		std::cout << std::endl;
	}
	return 0;
}