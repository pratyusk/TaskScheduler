# the compiler: g++
CC = g++

# flags: C++11 support + GDB + show all warnings
CFLAGS = -std=c++0x -g -Wall

# sqlite flag
SQLITE = -lsqlite3

# multi-threading
PTHREAD = -pthread -ldl

# object files
OBJECTS = TaskScheduler.o ConnectTCPServer.o CalculatePhysicalMemory.o

# executable: test case
TARGET = testPeriodicTaskScheduler

default: all

# make all
all: $(TARGET)

# g++ command
$(TARGET): $(OBJECTS) $(TARGET).cpp
	$(CC) $(CFLAGS) $(OBJECTS) -o $(TARGET) $(TARGET).cpp $(SQLITE) $(PTHREAD)
	chmod +x $(TARGET)
	chmod +x sampleTest.sh

# required object files
TaskScheduler.o: TaskScheduler.cpp TaskScheduler.h
	$(CC) $(CFLAGS) -c TaskScheduler.cpp

ConnectTCPServer.o: ConnectTCPServer.cpp ConnectTCPServer.h
	$(CC) $(CFLAGS) -c ConnectTCPServer.cpp

CalculatePhysicalMemory.o: CalculatePhysicalMemory.cpp CalculatePhysicalMemory.h
	$(CC) $(CFLAGS) -c CalculatePhysicalMemory.cpp

# clean executables
clean:
	$(RM) $(TARGET) $(TARGET).exe *.o *.db*
