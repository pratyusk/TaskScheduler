#include <iostream>
#include <string>
#include <algorithm>
#include <unistd.h>
// #include "TaskScheduler.h"
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

bool checkCommand(char command) {
	if (command != 'A' && command != 'C' && command != 'R' && command != 'E' && command != 'S') {
		std::cerr << "Please enter a valid command!" << std::endl;
		return false;
	}
	return true;
}

bool checkTask(std::string taskName, TaskScheduler &scheduler) {
	if (!scheduler.validateTaskName(taskName)) {
		std::cerr << "Please enter a valid task name! (Available: ConnectTCPServer, CalculatePhysicalMemory)" << std::endl;
		return false;
	}
	return true;
}

//
int main(int argc, char **argv) {
	TaskScheduler scheduler;
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
		while (!checkTask(taskName)) {
			checkStreamState(std::cin >> taskName, taskName);
		}
		std::transform(taskName.begin(), taskName.end(), taskName.begin(), ::tolower);
		if (taskName == "connecttcpserver" && command == 'A') {
			std::string addr, port;
			checkStreamState(std::cin >> addr, addr);
			checkStreamState(std::cin >> port, port);
		}
		if (command == 'A') {
			int timeInterval, timeToStart;
			checkStreamState(std::cin >> timeInterval, timeInterval);
			checkStreamState(std::cin >> timeToStart, timeToStart);
		}
		if (command	== 'R') {
			int newTimeInterval, timeToStart;
			checkStreamState(std::cin >> newTimeInterval, newTimeInterval);
			checkStreamState(std::cin >> timeToStart, timeToStart);
		}
		std::cout << std::endl;
	}
	return 0;
}