/*
	Create a TCP socket
*/
#include "socketUtils.h"
#include <stdbool.h>
#include <stdio.h>
#define LINELIMIT 200
#define NAMELIMIT 20

// #include <unistd.h>

void startListeningAndPrintMessagesOnNewThread(int fd);

DWORD WINAPI listenAndPrint(void* socketFD);

void readConsoleEntriesAndSendToServer(int socketFD);

void cleanupAndExit(int socketFD, int errorPresent, int errNo);

int main(int argc , char *argv[])
{
	WSADATA wsa;
	SOCKET s;
	struct sockaddr_in server;
	char server_reply[2000];
	int recv_size;

	printf("\nInitialising Winsock...\n");
	if (WSAStartup(MAKEWORD(2,2),&wsa) != 0)
	{
		printf("Failed. Error Code : %d",WSAGetLastError());
		return 1;
	}
	
	printf("Initialised.\n");
	
	//Create a socket
	if((s = socket(AF_INET , SOCK_STREAM , 0 )) == INVALID_SOCKET)
	{
		printf("Could not create socket : %d\n" , WSAGetLastError());
	}

	printf("Socket created.\n");
	
	
	// server.sin_addr.s_addr = inet_addr("74.125.235.20");
	server.sin_addr.s_addr = inet_addr("127.0.0.1");
	server.sin_family = AF_INET;
	server.sin_port = htons( 8000 );

	//Connect to remote server
	if (connect(s , (struct sockaddr *)&server , sizeof(server)) < 0)
	{
		printf("connect error\n");
		return 1;
	}
	
	printf("Connected\n");
	
	// //Receive a reply from the server
	// if((recv_size = recv(s , server_reply , 2000 , 0)) == SOCKET_ERROR)
	// {
	// 	printf("recv failed\n");
	// 	printf("recv_size: %i\n", recv_size);
	// 	// perror("recv");
	// 	getchar();
	// }
	
	// printf("Reply received\n");

	// //Add a NULL terminating character to make it a proper string before printing
	// server_reply[recv_size] = '\0';
	// printf("reply: %s, size of reply: %i", server_reply, recv_size);

	startListeningAndPrintMessagesOnNewThread(s);

    readConsoleEntriesAndSendToServer(s);

	close(s);
	return 0;
}

void readConsoleEntriesAndSendToServer(int socketFD) {
	char *name;
    char *line;
	int iResult;

	name = malloc(LINELIMIT + 1);
	line = malloc(NAMELIMIT + 1);

	if (line == NULL)
	{
		printf("Memory allocation failure");
		exit(-1);
	}

    printf("please enter your name?\n");
	fgets(name, LINELIMIT+1, stdin);

    printf("type and we will send(type exit)...\n");
    char buffer[250];

    while(fgets(line, LINELIMIT+1, stdin) != NULL)
    {

        sprintf(buffer,"%s:%s",name,line);

		if(strcmp(line,"exit")==0)
			break;

		// POTENTIAL BUG, does strlen stop at the null terminator of name?
		iResult =  send(socketFD,
						buffer,
						(int)strlen(buffer), 0);
		
		if (iResult == SOCKET_ERROR) {
			free(line);
			cleanupAndExit(socketFD, 1, iResult);
		}
    }

	free(line);
}

void startListeningAndPrintMessagesOnNewThread(int socketFD) {

    HANDLE id ;
    CreateThread(NULL, 0,listenAndPrint,&socketFD, 0, NULL);
}

DWORD WINAPI listenAndPrint(void* socketFD) {
    char buffer[LINELIMIT+1];
	int iResult;

    while (iResult = recv((int)socketFD,buffer,LINELIMIT + 1,0) > 0)
    {
        buffer[iResult] = 0;
        printf("Response was %s\n ",buffer);
        
    }

	if(iResult == 0)
	{
		printf("Connection was closed...");
	}
	else
	{
        printf("recv failed: %d\n", WSAGetLastError());
	}

    close((int)socketFD);
	return 0;
}

void cleanupAndExit(int socketFD, int errorPresent, int errNo)
{
	if (errorPresent == 1)
	{
		wprintf(L"send failed with error: %d\n", WSAGetLastError());
		closesocket(socketFD);
		WSACleanup();
		exit(-1);
	}
	else
	{
		printf("Gracefully shutting down...");
		closesocket(socketFD);
		WSACleanup();
		printf("bye!");
		exit(-1);
	}
	
}