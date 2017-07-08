# the compiler: g++
CC = g++

# flags: C++11 support + GDB + show all warnings
CFLAGS = -std=c++0x -g -Wall

# executable: test case
TARGET = testMurmur

default: all

# make all
all: $(TARGET)

# g++ command
$(TARGET): MurmurHash3.o MurmurClass.o $(TARGET).cpp
	$(CC) $(CFLAGS) MurmurHash3.o MurmurClass.o -o $(TARGET) $(TARGET).cpp
	chmod +x $(TARGET)

# required object files
MurmurHash3.o: MurmurHash3.cpp MurmurHash3.h
	$(CC) $(CFLAGS) -c MurmurHash3.cpp

MurmurClass.o: MurmurClass.cpp MurmurClass.h
	$(CC) $(CFLAGS) -c MurmurClass.cpp

# clean executables
clean:
	$(RM) $(TARGET) $(TARGET).exe *.o
