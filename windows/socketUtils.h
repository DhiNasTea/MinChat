#include <stdio.h>
#include <io.h>
#include<stdlib.h>
#include<winsock2.h>
#include <string.h>
#include <malloc.h>
#include <WS2tcpip.h>




struct sockaddr_in* createIPv4Address(char *ip, int port);

SOCKET createTCPIpv4Socket();

