#include "ConnectTCPServer.h"
#include <iostream>
double ConnectTCPServer(std::string addr, std::string port) {
    std::cout << "in ConnectTCPServer: addr " << addr << " " << port << std::endl;
	auto start = std::chrono::system_clock::now(); // start clock
    double connectTime = 0;
    int sockfd; // socket file descriptor
    struct addrinfo hints, *servinfo, *p;
    int rv; // status of getaddrinfo

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC; /* IPv4 or IPv6 */
    hints.ai_socktype = SOCK_STREAM; /* TCP */

    // char *addrCString = new char[100];
    // addrCString = const_cast<char *>(addr.c_str());
    // char *portCString = new char[100];
    // portCString = const_cast<char *>(port.c_str());
    // if ((rv = getaddrinfo(addrCString, portCString, &hints, &servinfo)) != 0) {
    if ((rv = getaddrinfo(addr.c_str(), port.c_str(), &hints, &servinfo)) != 0) {
        // delete [] addrCString;
        // delete [] portCString;
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
        return -1;
    }
    // delete [] addrCString;
    // delete [] portCString;
    // loop through all the results and connect to the first we can
    for(p = servinfo; p != NULL; p = p->ai_next) {
        if ((sockfd = socket(p->ai_family, p->ai_socktype,
                p->ai_protocol)) == -1) {
            perror("client: socket");
            continue;
        }

        if (connect(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
            close(sockfd);
            perror("client: connect");
            continue;
        }

        break;
    }

    if (p == NULL) {
        fprintf(stderr, "client: failed to connect\n");
        return -2;
    }

    freeaddrinfo(servinfo); // all done with this structure

    // extra code for an HTTP GET request
    /*
	char *buffer = "GET / HTTP/1.0\r\n\r\n";
	printf("SENDING: %s", buffer);
	printf("===\n");

	send(sockfd, buffer, strlen(buffer), MSG_DONTWAIT);

	char resp[MAX_BUF_SIZE];
	int len = read(sockfd, resp, MAX_BUF_SIZE - 1);
	resp[len] = '\0';
	printf("%s\n", resp); */

    auto end = std::chrono::system_clock::now(); // end clock
    std::chrono::duration<double, std::milli> elapsed = end - start; // time in milliseconds (may change to micro or nano)
    connectTime = static_cast<double>(elapsed.count()); // update time

    close(sockfd);

    return connectTime;
}