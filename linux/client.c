/*
	Create a TCP socket
*/
#include "socketUtils.h"

#pragma comment(lib,"ws2_32.lib") //Winsock Library

int main(int argc , char *argv[])
{
	WSADATA wsa;
	SOCKET s;
	struct sockaddr_in server;
	char *message , server_reply[2000];
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

	// print port
	printf("%i\n", htons(server.sin_port));

	//Connect to remote server
	if (connect(s , (struct sockaddr *)&server , sizeof(server)) < 0)
	{
		printf("connect error\n");
		return 1;
	}
	
	printf("Connected\n");
	
	// //Send some data
	// message = "GET / HTTP/1.1\r\n\r\n";
	// if( send(s , message , strlen(message) , 0) < 0)
	// {
	// 	printf("Send failed");
	// 	return 1;
	// }
	// printf("Data Send\n");
	
	//Receive a reply from the server
	if((recv_size = recv(s , server_reply , 2000 , 0)) == SOCKET_ERROR)
	{
		printf("recv failed\n");
		printf("recv_size: %i\n", recv_size);
		// perror("recv");
		getchar();
	}
	
	printf("Reply received\n");

	//Add a NULL terminating character to make it a proper string before printing
	server_reply[recv_size] = '\0';
	printf("reply: %s, size of reply: %i", server_reply, recv_size);

	return 0;
}