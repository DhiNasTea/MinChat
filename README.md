# MinChat
MinChat is a minimal implementation for public chatrooms. 

Only one computer is needed to host a server for users to connect to it and start chatting. This project uses C sockets and currently offers binairies for Windows machine.

# Compilation
### Windows
To compile the code in the current state, execute the following line in the command prompt

Client side:
```bash
gcc client.c socketUtils.c -o minchat_client.exe -lwsock32 -lws2_32
```

Server side:
```bash
gcc -o minchat_server.exe server.c -lwsock32
```

# Usage
### Client Side

> **_Note:_** The Server needs to be ran first for clients to be able to connect to it. 

Connect to a server that's running locally by default, run the executable (port 8765 is the default):
```bash
minchat_client.exe
```
To connect to a remote server, specify the IP address and the port as arguments:
```bash
minchat_client.exe <ip address> <port number>
```

### Server Side
Run a server locally with the default port 8765 and maximum of 10 concurrent connections:
```bash
minchat_server.exe
```
To specify the port to use as well as the number of user, add these as arguments:
```bash
minchat_server.exe <port number> <max concurrent users>
```


If default parameters are used, the clients will search for the server on the localhost. 

If the server is hosted on the same network, the IP address of the computer hosting the server can be used by the clients. For a computer accross another network, the IP address needs to be static in order for the connections to be complete.
