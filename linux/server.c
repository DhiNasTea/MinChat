/*
	Live Server on port 8888
*/
#include "socketUtils.h"

#pragma comment(lib,"ws2_32.lib") //Winsock Library

#define BUF_SIZE 500

int main(int argc , char *argv[])
{
	WSADATA wsa;
	SOCKET s , new_socket;
	struct sockaddr_in server , client;
	int c;
	char *message;

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
	server.sin_port = htons( 8000 );

	// print port
	printf("%i\n", htons(server.sin_port));
	
	//Bind
	if( bind(s ,(struct sockaddr *)&server , sizeof(server)) == SOCKET_ERROR)
	{
		printf("Bind failed with error code : %d\n" , WSAGetLastError());
		exit(EXIT_FAILURE);
	}
	
	printf("Bind done\n");

	//Listen to incoming connections
	listen(s , 3);
	
	//Accept and incoming connection
	printf("Waiting for incoming connections...\n");
	
	c = sizeof(struct sockaddr_in);
	
	while( (new_socket = accept(s , (struct sockaddr *)&client, &c)) >= 0 )
	{
		printf("Connection accepted\n");
		
		//Reply to the client
        message = "Hello Client , I have received your connection. But I have to go now, bye\n";
		// char buf[BUF_SIZE] = "Hello Client , I have received your connection. But I have to go now, bye\n\0";
		int res = send(new_socket , message , strlen(message) , 0);
        if (res < 0)
        {
            printf("an error occured while sending\n");
        }
	}
	
	if (new_socket == INVALID_SOCKET)
	{
		printf("accept failed with error code : %d\n" , WSAGetLastError());
		return 1;
	}

	closesocket(s);
	WSACleanup();
	
	return 0;
}