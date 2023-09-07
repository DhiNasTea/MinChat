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
	
	server.sin_family = AF_INET;
	if (argc == 3)
	{
		server.sin_addr.s_addr = inet_addr(argv[1]);
		server.sin_port = htons( atoi(argv[2]) );
	}
	else
	{
		server.sin_addr.s_addr = inet_addr("127.0.0.1");
		server.sin_port = htons( 8765 );
	}
	

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

	name = malloc(NAMELIMIT + 1);
	line = malloc(LINELIMIT + 1);

	if (line == NULL)
	{
		printf("Memory allocation failure");
		exit(-1);
	}

    printf("please enter your name?\n");
	fgets(name, NAMELIMIT+1, stdin);
	name[strcspn(name, "\n")] = 0;

    printf("type and we will send(type exit)...\n");
    char buffer[LINELIMIT+NAMELIMIT+1];

    while(fgets(line, LINELIMIT+1, stdin) != NULL)
    {
		line[strcspn(line, "\n")] = 0;
        sprintf(buffer,"%s > %s",name,line);
		// printf("before sending: %s", buffer);

		if(strcmp(line,"exit")==0)
			break;

		// TODO: POTENTIAL BUG, does strlen stop at the null terminator of name?
		iResult =  send(socketFD,
						buffer,
						(int)strlen(buffer),
						0);
		
		if (iResult == SOCKET_ERROR) {
			free(line);
			cleanupAndExit(socketFD, 1, iResult);
		}
    }

	free(line);
}

void startListeningAndPrintMessagesOnNewThread(int socketFD) {

    HANDLE id ;
	int * argsThread[1];
	argsThread[0] = (int *) malloc(sizeof(int));

	if (argsThread[0] == NULL)
	{
		// If the array allocation fails, the system is out of memory
        // so there is no point in trying to print an error message.
        // Just terminate execution.
    	ExitProcess(2);
	}

	* argsThread[0] = socketFD;
    CreateThread(NULL, 0,listenAndPrint, argsThread[0], 0, NULL);
}

DWORD WINAPI listenAndPrint(void* socketFD) {
	// check if size should be bigger is username is sent
    char buffer[LINELIMIT+NAMELIMIT+1];
	int iResult;
	int * argsThread = (int *) socketFD;

    while (true)
    {
		iResult = recv(*argsThread,buffer,LINELIMIT + NAMELIMIT + 1,0);
		if (iResult <= 0)
		{
			break;
		}
        buffer[iResult] = 0;
        printf("%s\n ",buffer);
        
    }
	if(iResult == 0)
	{
		printf("Connection was closed...");
	}
	else
	{
        printf("recv failed: %d\n", WSAGetLastError());
	}

    close(*argsThread);
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