# 42cursus-webserv




## Topics

**Web server**: A computer program that serves requested HTML pages or files.

- [What is a Server? (Deepdive)](https://youtu.be/VXmvM2QtuMU) **(Recommended)**
- [Web Server Concepts and Examples](https://www.youtube.com/watch?v=9J1nJOivdyw&t=313s)

- [Coding a Web Server in 25 Lines - Computerphile](https://www.youtube.com/watch?v=7GBlCinu9yg)

**Endianess**: The order in which bytes are stored in memory.
- **Big-endian**: Most significant byte is stored at the lowest memory address.
- **Little-endian**: Least significant byte is stored at the lowest memory address.

	- [Computerphile - Endianness Explained With an Egg](https://youtu.be/NcaiHcBvDR4)
	- [Computerphile - Lecture 22. Big Endian and Little Endian](https://youtu.be/T1C9Kj_78ek)
	- [What is Endianness? Big-Endian vs Little-Endian Explained with Examples](https://www.freecodecamp.org/news/what-is-endianness-big-endian-vs-little-endian/)
	- [geeksforgeeks - What is Endianness? Big-Endian & Little-Endian](https://www.geeksforgeeks.org/little-and-big-endian-mystery/)

**Networking Basics**:

1. **TCP/IP Protocol Suite**: A set of protocols that allows two or more devices to communicate over a network.

	- **IP (Internet Protocol)**:
		- An IP address is a unique identifier for a device on a network.
		- IPv4 addresses are 32-bit numbers usually represented in dotted-decimal format (e.g., 192.168.1.1).
		- IPv6 addresses are 128-bit numbers represented in hexadecimal (e.g., 2001:0db8:85a3:0000:0000:8a2e:0370:7334).
	- **TCP (Transmission Control Protocol)**:
		- A connection-oriented protocol that provides reliable, ordered, and error-checked delivery of a stream of bytes.
		- **Three-way handshake**:
			1. Client sends a SYN packet to the server.
			2. Server sends a SYN-ACK packet to the client.
			3. Client sends an ACK packet to the server.
		- **Four-way handshake**:
			1. Client sends a FIN packet to the server.
			2. Server sends an ACK packet to the client.
			3. Server sends a FIN packet to the client.
			4. Client sends an ACK packet to the server.
	- **UDP (User Datagram Protocol)**:
		- A connectionless protocol that provides unreliable, unordered, and error-checked delivery of a stream of bytes.
		- **Connectionless**: No connection is established before data is sent.
		- **Unreliable**: No guarantee that data will be delivered.
		- **Unordered**: No guarantee that data will be received in the order it was sent.
	- **Ports**:
		- A port is a communication endpoint that identifies a specific process or service on a host.
		- Ports are identified by a 16-bit number ranging from 0 to 65535.
		- **Well-known ports**:
			- Ports ranging from 0 to 1023 are reserved for system services.
		- **Registered ports**:
			- Ports ranging from 1024 to 49151 are registered with the IANA.
		- **Dynamic ports**:
			- Ports ranging from 49152 to 65535 are used for private or temporary purposes.
	- **Socket**:
		- A socket is an endpoint for communication between two machines.
		- A socket is identified by an IP address and a port number.
		- **Server socket**:
			- A server socket listens for incoming connections.
		- **Client socket**:
			- A client socket initiates a connection to a server socket.

- [What is a Protocol? (Deepdive)](https://www.youtube.com/watch?v=d-zn-wv4Di8&t=246s)

2. **Socket Programming**: The process of writing programs that communicate with other programs across a network using sockets.

	- **Server-Client Model**:
		- A server is a program that listens for incoming connections.
		- A client is a program that initiates a connection to a server.
	- **Socket**:
		- A socket is an endpoint for communication between two machines.
		- A socket is identified by an IP address and a port number.
		- **Server socket**:
			- A server socket listens for incoming connections.
		- **Client socket**:
			- A client socket initiates a connection to a server socket.
	- **TCP Server-Client**:
		- **Server**:
			1. Create a socket using the `socket()` system call.
				```c
				int sockfd = socket(AF_INET, SOCK_STREAM, 0);
				
				// AF_INET: Address family for IPv4.
				// SOCK_STREAM: Type (TCP for reliable, connection-oriented service).
				// 0: Protocol (0 lets the system choose the appropriate protocol).
				```
			2. Bind the socket to an address using the `bind()` system call.
				```c
				struct sockaddr_in addr;
				addr.sin_family = AF_INET;
				addr.sin_addr.s_addr = INADDR_ANY;
				addr.sin_port = htons(8080);

				bind(sockfd, (struct sockaddr *)&addr, sizeof(addr));

				// struct sockaddr_in: Structure for IPv4 addresses.
				// sin_family: Address family for IPv4.
				// AF_INET: Address family for IPv4.
				// sin_addr.s_addr: IP address of the host.
				// sin_port: Port number.
				// INADDR_ANY: Accept connections to any IP address.
				// htons(): Convert port number to network byte order.
				```
			3. Listen for incoming connections using the `listen()` system call.
				```c
				listen(sockfd, SOMAXCONN);

				// SOMAXCONN: Maximum number of pending connections in the listen queue.
				```
			4. Accept a connection using the `accept()` system call.
				```c
				int new_socket = accept(sockfd, (struct sockaddr *)&address, &addrlen);

				// accept(): Waits for an incoming connection and returns a new socket descriptor.
				// new_socket: Socket descriptor for the new connection.
				```
			5. Send and receive data using the `send()` and `recv()` system calls.
				```c
				send(new_socket, "Hello, World!", 13, 0);
				recv(new_socket, buffer, 1024, 0);
				// send(): Sends data to the connected socket.
				// recv(): Receives data from the connected socket.
				// buffer: Buffer to store received data.
				```
		- **Client**:
			1. Create a socket using the `socket()` system call.
			2. Connect to a server using the `connect()` system call.
			3. Send and receive data using the `send()` and `recv()` system calls.
	- **UDP Server-Client**:
		- **Server**:
			1. Create a socket using the `socket()` system call.
			2. Bind the socket to an address using the `bind()` system call.
			3. Send and receive data using the `sendto()` and `recvfrom()` system calls.
		- **Client**:
			1. Create a socket using the `socket()` system call.
			2. Send and receive data using the `sendto()` and `recvfrom()` system calls.

	- [Sockets in Operating System](https://youtu.be/uagKTbohimU)
	- [Socket Programming in C/C++ - GeeksforGeeks](https://www.geeksforgeeks.org/socket-programming-cc/)
	- [Socket Programming in C - Tutorialspoint](https://www.tutorialspoint.com/unix_sockets/socket_server_example.htm)
