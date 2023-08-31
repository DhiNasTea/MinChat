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
Connect to a server that's running locally by default, specify the port in the first argument and optionally the IP address to connect to:
```bash
minchat_server.exe <port number> <(optional) ip address>
```

### Server Side
Run a server locally on a specified port, defaults to three connections maximum:
```bash
minchat_server.exe <port number> <(optional) max connections>
```

