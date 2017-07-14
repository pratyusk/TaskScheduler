# Periodic Task Scheduler #
I implemented the periodic task scheduler using C++11 libraries and resources as mentioned
in the references. The development is done on a Linux platform with GCC version
g++ (GCC) 4.8.5 20150623 (Red Hat 4.8.5-11)

## Usage ##
	$ make clean
	$ make
	$ ./sampleTesh.sh

This will compile the classes and the test case and run the test case.
The test case can be edited from `sampleInput.txt`.

The tasks are represented by their task names and metric to store and update
(currently implemented calculating time to connect to a TCP server and
calculating the physical memory used by the current process).
Class `TaskScheduler` has `addTask`, `cancelTask` and `rescheduleTask` methods.
The scheduler can run any task with any signature (signature needs to be declared
in the test case/ application). It is assumed that the task will return a double
or int type metric. For more than one metric, the output should be a vector of metric.

## Notes ##
1. The task names and their metric names make up the primary key. This is because for this design,
the same process can have more than one metric. Each task-metric pair gets a unique row.
2. The units of all metrics is `double` or `REAL`.
3. The task names are case-insensitive.
4. Commands A, C, R, S, E are case-insensitive.
5. Commands are :
	* Add Task : 'A <task_name> <metric_name> <metric_units> <time_interval> `[<addr>] [<port>]`'
		- NOTE : For ConnectTCPServer, provide address and port number (whitespace separated)
		- time interval in seconds
	* Cancel Task : 'C <task_name> <metric_name>'
	* Reschedule Task : 'R <task_name> <metric_name> <new_time_interval>'
		- NOTE : All times are in seconds
	* Sleep : 'S <wait_time>(in seconds)'
	* Exit test case : 'E'

## Testing ##
Currently tested for:
1. Adding new task
2. Cancelling task
3. Rescheduling task
4. Adding task back
5. Adding a task that is already running
6. Cancelling a task that is not running
