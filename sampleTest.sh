#!/bin/bash
clear

# Test case
# Please modify sampleInput.txt to change test case
echo "--- Running Test Case ---"
echo "Add ConnectTCPServer to google.com port 80 with 3 seconds interval"
echo "Add CalculatePhysicalMemory with 5 seconds interval"
echo "Wait for 15 seconds"
echo "Cancel ConnectTCPServer task"
echo "Reschedule CalculatePhysicalMemory task with 2 seconds interval"
echo "Wait 8 seconds"
echo "Resume ConnectTCPServer task with 5 seconds interval"
echo "Wait 10 seconds"
echo "Exit test case"
./testPeriodicTaskScheduler < sampleInput.txt
echo "Test case ended!"

# Clean up
rm *.db*