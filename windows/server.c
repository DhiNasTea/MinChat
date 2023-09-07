#include "socketUtils.h"
#include <stdbool.h>
// #include <unistd.h>
// #include <pthread.h>
#define LINELIMIT 200
#define NAMELIMIT 20
#define MAXUSERS 10

struct AcceptedSocket
{
    int acceptedSocketFD;
	int * ptrAcceptedSocketFD;
    struct sockaddr_in address;
    int error;
    bool acceptedSuccessfully;
};

struct AcceptedSocket * acceptIncomingConnection(int serverSocketFD);
void acceptNewConnectionAndReceiveAndPrintItsData(int serverSocketFD);
DWORD WINAPI receiveAndPrintIncomingData(void * socketFD);
void startAcceptingIncomingConnections(int serverSocketFD);
void receiveAndPrintIncomingDataOnSeparateThread(struct AcceptedSocket *pSocket);
void sendReceivedMessageToTheOtherClients(char *buffer,int socketFD);

struct AcceptedSocket acceptedSockets[10] ;
int acceptedSocketsCount = 0;

struct AcceptedSocket * acceptIncomingConnection(int serverSocketFD) {
    struct sockaddr_in  clientAddress ;
    int clientAddressSize = sizeof (struct sockaddr_in);
    int clientSocketFD = accept(serverSocketFD,(struct sockaddr *)&clientAddress,&clientAddressSize);

    struct AcceptedSocket* acceptedSocket = malloc(sizeof (struct AcceptedSocket));
    acceptedSocket->address = clientAddress;
    acceptedSocket->acceptedSocketFD = clientSocketFD;
	acceptedSocket->ptrAcceptedSocketFD = &acceptedSocket->acceptedSocketFD;
    acceptedSocket->acceptedSuccessfully = clientSocketFD>0;

    if(!acceptedSocket->acceptedSuccessfully)
        acceptedSocket->error = clientSocketFD;

    return acceptedSocket;
}


void startAcceptingIncomingConnections(int serverSocketFD) {

    while(true)
    {
        struct AcceptedSocket* clientSocket  = acceptIncomingConnection(serverSocketFD);
        acceptedSockets[acceptedSocketsCount++] = *clientSocket;

        receiveAndPrintIncomingDataOnSeparateThread(clientSocket);
    }
}

void receiveAndPrintIncomingDataOnSeparateThread(struct AcceptedSocket *pSocket) {

    HANDLE id;
	int * argsThread[1];
	argsThread[0] = (int *) malloc(sizeof(int));

	if (argsThread[0] == NULL)
	{
		// If the array allocation fails, the system is out of memory
        // so there is no point in trying to print an error message.
        // Just terminate execution.
    	ExitProcess(2);
	}

	argsThread[0] = &pSocket->acceptedSocketFD;

	CreateThread(NULL, 0,receiveAndPrintIncomingData, argsThread[0], 0, NULL);
}

DWORD WINAPI receiveAndPrintIncomingData(void * socketFD) {
    char buffer[LINELIMIT+NAMELIMIT+1];
	int iResult;
	int * argsThread = (int *) socketFD;

	// printf("socket: %i, socket addr: %i\n", *argsThread, argsThread);
	printf("Accepted a new connection\n");

	while (true)
    {
		iResult = recv(*argsThread,buffer,LINELIMIT + NAMELIMIT + 1,0);
		if (iResult <= 0)
		{
			break;
		}

        buffer[iResult] = 0;
        printf("%s\n", buffer);
        
		sendReceivedMessageToTheOtherClients(buffer,*argsThread);
    }

	if(iResult == 0)
	{
		printf("Connection was closed...\n");
	}
	else
	{
        printf("recv failed: %d\n", WSAGetLastError());
	}

    close(*argsThread);
}

void sendReceivedMessageToTheOtherClients(char *buffer,int socketFD) {

    for(int i = 0 ; i<acceptedSocketsCount ; i++)
        if(acceptedSockets[i].acceptedSocketFD !=socketFD)
        {
            send(acceptedSockets[i].acceptedSocketFD,buffer, strlen(buffer),0);
        }

}




int main(int argc , char *argv[])
{
	WSADATA wsa;
	SOCKET s , new_socket;
	struct sockaddr_in server , client;
	int c;
	char *message;
	int nbUsers = MAXUSERS;

	printf("\nInitialising Winsock...");
	if (WSAStartup(MAKEWORD(2,2),&wsa) != 0)
	{
		printf("Failed. Error Code : %d",WSAGetLastError());
		return 1;
	}
	
	printf("Initialised.\n");
	
	//Create a socket
	if((s = socket(AF_INET , SOCK_STREAM , 0 )) == INVALID_SOCKET)
	{
		printf("Could not create socket : %d" , WSAGetLastError());
	}

	printf("Socket created.\n");
	
	//Prepare the sockaddr_in structure
	server.sin_family = AF_INET;
	server.sin_addr.s_addr = INADDR_ANY;
	if (argc == 3)
	{
		server.sin_port = htons( atoi(argv[1]) );
		nbUsers = atoi(argv[2]);
	}
	else
	{
		server.sin_port = htons( 8765 );
	}
	
	//Bind
	if( bind(s ,(struct sockaddr *)&server , sizeof(server)) == SOCKET_ERROR)
	{
		printf("Bind failed with error code : %d\n" , WSAGetLastError());
		exit(EXIT_FAILURE);
	}
	printf("Bind done\n");


	//Listen to incoming connections
	listen(s , nbUsers);
	//Accept and incoming connection
	printf("Waiting for incoming connections...\n");

	startAcceptingIncomingConnections(s);

	shutdown(s, SD_BOTH);

	// TODO: Check if closesocket is needed after shutdown
	closesocket(s);
	WSACleanup();
	
	return 0;
}