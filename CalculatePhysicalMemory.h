#ifndef _CALCULATEPHYSICALMEMORY_H_
#define _CALCULATEPHYSICALMEMORY_H_

#include <iostream>
#include "stdlib.h"
#include "stdio.h"
#include "string.h"
#include <string>

/******************************************************************************
 * Author: Pratyush Kumar
 * Last Updated: July 14, 2017
 * Purpose: Calculate physical memory used by current process
 *****************************************************************************/


// calculates the physical memory of the current process in kB
// returns the physical memory of the process in kB on success, -1 on failure
int CalculatePhysicalMemory();

#endif