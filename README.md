# TaskScheduler
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

## Notes ##
1. The task names and their metric names make up the primary key. This is because for this design,
the same process can have more than one metric. Each task-metric pair gets a unique row.
2. The units of all metrics is `double` or `REAL`.
3. The task names are case-insensitive.
4. Commands A, C, R, S, E are case-insensitive.
5. Commands are :
	a.

## Testing ##
Currently tested for:
1. Adding new task
2. Cancelling task
3. Rescheduling task
4. Adding task back
5. Adding a task that is already running
6. Cancelling a task that is not running
7. When a task takes longer than the interval to complete
8. Precision issues