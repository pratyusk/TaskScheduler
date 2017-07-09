#include <iostream>
#include "TaskScheduler.h"
#include "ConnectTCPServer.h"
#include "CalculatePhysicalMemory.h"

//
int main(int argc, char **argv) {
	std::cout << "*** Tasks available : CalculatePhysicalMemory, ConnectTCPServer ***" << std::endl;
	std::cout << "\tsubstitute for <task_name>: 1 - ConnectTCPServer" << std::endl;
	std::cout << "\tsubstitute for <task_name>: 2 - CalculatePhysicalMemory\n" << std::endl;
	std::cout << "*** Commands : ***" << std::endl;
	std::cout << "\tAdd Task : 'A <task_name>'" << std::endl;
	std::cout << "\t\tNOTE : For ConnectTCPServer, provided address and port number (whitespace separated)" << std::endl;
	std::cout << "\tCancel Task : 'C <task_name>'" << std::endl;
	std::cout << "\tReschedule Task : 'R <task_name> <new_time_interval> <time_to_start_from_now>'" << std::endl;
	std::cout << "\t\tNOTE : All times are in seconds" << std::endl;
	std::cout << "\tExit test case : 'E'\n" << std::endl;
	while (1) {
		char
		std::cin >>
	}
}