# the compiler: g++
CC = g++

# flags: C++11 support + GDB + show all warnings
CFLAGS = -std=c++0x -g -Wall

# executable: test case
TARGET = testPeriodicTaskScheduler

default: all

# make all
all: $(TARGET)

# g++ command
$(TARGET): TaskScheduler.o ConnectTCPServer.o CalculatePhysicalMemory.o $(TARGET).cpp
	$(CC) $(CFLAGS) TaskScheduler.o ConnectTCPServer.o CalculatePhysicalMemory.o -o $(TARGET) $(TARGET).cpp
	chmod +x $(TARGET)

# required object files
TaskScheduler.o: TaskScheduler.cpp TaskScheduler.h
	$(CC) $(CFLAGS) -c TaskScheduler.cpp

ConnectTCPServer.o: ConnectTCPServer.cpp ConnectTCPServer.h
	$(CC) $(CFLAGS) -c ConnectTCPServer.cpp

CalculatePhysicalMemory.o: CalculatePhysicalMemory.cpp CalculatePhysicalMemory.h
	$(CC) $(CFLAGS) -c CalculatePhysicalMemory.cpp

# clean executables
clean:
	$(RM) $(TARGET) $(TARGET).exe *.o
