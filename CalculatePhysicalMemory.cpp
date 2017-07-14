#include "CalculatePhysicalMemory.h"

// This assumes that a digit will be found and the line ends in " Kb"
int parseLine(char* line) {
    int i = strlen(line);
    const char *p = line;
    while (*p < '0' || *p > '9') p++; // find where number begins
    line[i-3] = '\0'; // remove 3 characters for " Kb"
    i = atoi(p); // convert to int
    return i;
}

int CalculatePhysicalMemory(std::string a, std::string b) { // inputs are dummy
    FILE* file = fopen("/proc/self/status", "r"); // file with process status
    int result = -1;
    char line[128];

    while (fgets(line, 128, file) != NULL) {
        if (strncmp(line, "VmRSS:", 6) == 0) { // VmRSS for physical memory
            result = parseLine(line);
            break;
        }
    }

    fclose(file);
    return result;
}