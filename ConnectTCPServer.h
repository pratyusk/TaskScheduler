#ifndef _CONNECTTCPSERVER_H_
#define _CONNECTTCPSERVER_H_

/******************************************************************************
 * Author: Pratyush Kumar
 * Last Updated: July 13, 2017
 * Purpose: Connect to a TCP server and calculate the time taken
 *****************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <chrono>
#include <string>

const unsigned int MAX_BUF_SIZE = 2000; // set to maximum bytes to receive

// connects to a TCP server
// address is provided in addr and port number provided in port
// time taken to connect is calculated in milliseconds (wall-clock time)
// Optional code provided for HTTP GET request
// returns connectTime on success, -1 on failure during getaddrinfo, -2 on failure to connect
double ConnectTCPServer(std::string addr, std::string port);

#endif

