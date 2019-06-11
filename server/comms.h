#ifndef __COMMS__
#define __COMMS__
#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>

int startServer(char* port, int* sockfd);
#endif