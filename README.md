# 42cursus-webserv

## Overview

This project involves developing a robust HTTP server in C++98, designed to handle basic web traffic and serve static content. Our server will conform to HTTP/1.1 standards and utilize non-blocking I/O operations for efficient resource management.


## Project Goals

- Implement core HTTP methods: GET, POST, DELETE.
- Support non-blocking I/O and socket multiplexing.
- Serve static content (HTML, CSS, JS).
- Ensure robust error handling and generate default error pages.
- Create a customizable configuration system.

## Team Members and Roles

- **Hassan Sobane** (hsobane):
- **Bilal Eddinaoui** (beddinao): 
- **Zakaria Elhajoui** (zelhajou):

## Project Timeline

|Phase|Timeline|Objectives
|---|---|---|
Planning | Week 1 |Define scope, architecture design, task breakdown.
Development	| Week 2-6 |Core functionality implementation.
Testing |Week 7-8 |Unit testing, integration testing, stress testing.

## Technical Requirements

- **Programming Language**: C++98
- **Development Tools**: Git, Make, GCC, Valgrind
- **Testing Tools**: Tool for stress testing

## Directory Structure

```
webserv/
│
├── src/                     # Source files for the server
│   ├── main.cpp             # Main application entry point
│   │
│   ├── server/              # Core server functionalities
│   │   ├── Server.cpp       # Implementation of the server class
│   │   └── Server.hpp       # Definition of the server class
│   │
│   ├── http/                # HTTP protocol handling
│   │   ├── RequestHandler.cpp   # Handles HTTP requests
│   │   ├── RequestHandler.hpp   # Definition of request handler
│   │   ├── Request.cpp          # HTTP request parsing and handling
│   │   ├── Request.hpp          # Definitions for HTTP request
│   │   ├── Response.cpp         # Constructs HTTP responses
│   │   └── Response.hpp         # Definitions for HTTP response
│   │
│   ├── network/             # Networking and socket management
│   │   ├── Socket.cpp       # Socket class implementation
│   │   ├── Socket.hpp       # Socket class definition
│   │   ├── Connection.cpp   # Manages individual connections
│   │   └── Connection.hpp   # Connection class definition
│   │
│   ├── config/              # Server configuration management
│   │   ├── ConfigParser.cpp # Parses configuration files
│   │   └── ConfigParser.hpp # Definitions for the config parser
│   │
│   └── utils/               # Utility functions and classes
|
├── tests/                   # Unit and integration tests
│   ├── test_main.cpp        # Main file for running tests
│   └── ServerTest.cpp       # Tests for server functionalities
│
├── config/                  # Configuration files for the server
│   ├── server.conf          # Server configuration settings
│   └── mime.types           # MIME types for response headers
│
├── docs/                    # Documentation files
│   ├── architecture.md      # Detailed architecture description
│   └── setup.md             # Setup and installation instructions
│
├── include/                 # Global header files
│   └── common.hpp           # Common header file for global definitions
│
├── assets/                  # Static files served by the server
│   ├── index.html           # Homepage
│   ├── style.css            # CSS for the homepage
│   └── script.js            # JavaScript for client-side logic
│
├── Makefile                 # Makefile for compiling the project
└── README.md                # Project overview and setup instructions
```


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
				```c
				int sockfd = socket(AF_INET, SOCK_STREAM, 0);

				// AF_INET: Address family for IPv4.
				// SOCK_STREAM: Type (TCP for reliable, connection-oriented service).
				// 0: Protocol (0 lets the system choose the appropriate protocol).
				```
			2. Connect to a server using the `connect()` system call.
				```c
				struct sockaddr_in addr;
				addr.sin_family = AF_INET;
				addr.sin_addr.s_addr = inet_addr("a.b.c.d");
				addr.sin_port = htons(8080);

				connect(sockfd, (struct sockaddr *)&addr, sizeof(addr));

				// inet_addr(): Converts an IP address in dotted-decimal notation to a long.
				// htons(): Convert port number to network byte order.
				```
			3. Send and receive data using the `send()` and `recv()` system calls.
				```c
				send(sockfd, "Hello, World!", 13, 0);
				recv(sockfd, buffer, 1024, 0);

				// send(): Sends data to the connected socket.
				// recv(): Receives data from the connected socket.
				// buffer: Buffer to store received data.
				```
	- **UDP Server-Client**:
		- **Server**:
			1. Create a socket using the `socket()` system call.
				```c
				int sockfd = socket(AF_INET, SOCK_DGRAM, 0);

				// AF_INET: Address family for IPv4.
				// SOCK_DGRAM: Type (UDP for unreliable, connectionless service).
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
				// bind(): Assigns the address specified by addr to the socket sockfd.
				```
			3. Send and receive data using the `sendto()` and `recvfrom()` system calls.
				```c
				sendto(sockfd, "Hello, World!", 13, 0, (struct sockaddr *)&addr, sizeof(addr));
				recvfrom(sockfd, buffer, 1024, 0, (struct sockaddr *)&addr, &addrlen);

				// sendto(): Sends data to a specific address.
				// recvfrom(): Receives data from a specific address.
				```
		- **Client**:
			1. Create a socket using the `socket()` system call.
				```c
				int sockfd = socket(AF_INET, SOCK_DGRAM, 0);

				// AF_INET: Address family for IPv4.
				// SOCK_DGRAM: Type (UDP for unreliable, connectionless service).
				// 0: Protocol (0 lets the system choose the appropriate protocol).
				```
			2. Send and receive data using the `sendto()` and `recvfrom()` system calls.
				```c
				sendto(sockfd, "Hello, World!", 13, 0, (struct sockaddr *)&addr, sizeof(addr));
				recvfrom(sockfd, buffer, 1024, 0, (struct sockaddr *)&addr, &addrlen);

				// sendto(): Sends data to a specific address.
				// recvfrom(): Receives data from a specific address.
				```
3. **TCP/IP Communication**: The process of establishing a connection between two devices using the TCP/IP protocol suite.

	- **Three-way handshake**:
		1. Client sends a SYN packet to the server.
		2. Server sends a SYN-ACK packet to the client.
		3. Client sends an ACK packet to the server.
	- **Four-way handshake**:
		1. Client sends a FIN packet to the server.
		2. Server sends an ACK packet to the client.
		3. Server sends a FIN packet to the client.
		4. Client sends an ACK packet to the server.
	- **Connection Establishment**:
		1. Client sends a SYN packet to the server.
		2. Server sends a SYN-ACK packet to the client.
		3. Client sends an ACK packet to the server.
	- **Connection Termination**:
		1. Client sends a FIN packet to the server.
		2. Server sends an ACK packet to the client.
		3. Server sends a FIN packet to the client.
		4. Client sends an ACK packet to the server.
	- **TCP Header**:
		- **Source Port**: 16 bits.
		- **Destination Port**: 16 bits.
		- **Sequence Number**: 32 bits.
		- **Acknowledgment Number**: 32 bits.
		- **Data Offset**: 4 bits.
		- **Reserved**: 6 bits.
		- **Flags**: 6 bits.
		- **Window Size**: 16 bits.
		- **Checksum**: 16 bits.
		- **Urgent Pointer**: 16 bits.
		- **Options**: Variable length.
		- **Padding**: Variable length.
	- **TCP Flags**:
		- **SYN (Synchronize)**: Initiates a connection.
		- **ACK (Acknowledgment)**: Acknowledges received data.
		- **FIN (Finish)**: Terminates a connection.
		- **RST (Reset)**: Resets a connection.
		- **PSH (Push)**: Pushes data to the application layer.
		- **URG (Urgent)**: Indicates urgent data.
	- **TCP State Diagram**:
		- **CLOSED**: No connection.
		- **LISTEN**: Waiting for a connection request.
		- **SYN-SENT**: Sent a connection request.
		- **SYN-RECEIVED**: Received a connection request.
		- **ESTABLISHED**: Connection established.
		- **FIN-WAIT-1**: Sent a connection termination request.
		- **FIN-WAIT-2**: Received an acknowledgment for the termination request.
		- **CLOSE-WAIT**: Received a connection termination request.
		- **CLOSING**: Sent an acknowledgment for the termination request.
		- **LAST-ACK**: Received a connection termination request acknowledgment.
		- **TIME-WAIT**: Waiting for delayed packets.
	
4. **Non-blocking I/O and Multiplexing**: The process of handling multiple I/O operations simultaneously without blocking.

	- **Non-blocking I/O**:
		- **Blocking I/O**:
			- The process waits until the I/O operation is complete.
		- **Non-blocking I/O**:
			- The process continues executing while the I/O operation is in progress.
				```c
				fcntl(sockfd, F_SETFL, O_NONBLOCK);

				// fcntl(): Performs operations on file descriptors.
				```
		- **Multiplexing**:
			- **select()**:
				- Monitors multiple file descriptors for I/O readiness.
				- **Read Set**:
					- Contains file descriptors that are ready for reading.
				- **Write Set**:
					- Contains file descriptors that are ready for writing.
				- **Error Set**:
					- Contains file descriptors that have errors.
				- **Timeout**:
					- Specifies the maximum time to wait for an event.
				- **select()** returns the number of ready file descriptors.
				- **FD_ISSET()**:
					- Checks if a file descriptor is in a set.
				- **FD_SET()**:
					- Adds a file descriptor to a set.
				- **FD_CLR()**:
					- Removes a file descriptor from a set.
				- **FD_ZERO()**:
					- Clears a set.
				```c
				fd_set readfds;
				FD_ZERO(&readfds);
				FD_SET(sockfd, &readfds);

				select(sockfd + 1, &readfds, NULL, NULL, NULL);

				if (FD_ISSET(sockfd, &readfds)) {
					// sockfd is ready for reading.
				}

				// FD_ZERO: Clears a set.
				// FD_SET: Adds a file descriptor to a set.
				// FD_CLR: Removes a file descriptor from a set.
				// FD_ISSET: Checks if a file descriptor is in a set.
				```
			- **poll()**:
				- Monitors multiple file descriptors for I/O readiness.
				- **Timeout**:
					- Specifies the maximum time to wait for an event.
				- **poll()** returns the number of ready file descriptors.
				```c
				struct pollfd fds;
				fds.fd = sockfd;
				fds.events = POLLIN;

				poll(&fds, 1, -1);

				if (fds.revents & POLLIN) {
					// sockfd is ready for reading.
				}

				// struct pollfd: Structure for poll events.
				// fds.fd: File descriptor to monitor.
				// fds.events: Events to monitor.
				// fds.revents: Events that occurred.
				```
			- **epoll()**:
				- Monitors multiple file descriptors for I/O readiness.
				- **Timeout**:
					- Specifies the maximum time to wait for an event.
				- **epoll()** returns the number of ready file descriptors.
				```c
				int epfd = epoll_create(1);
				struct epoll_event event;
				event.events = EPOLLIN;
				event.data.fd = sockfd;

				epoll_ctl(epfd, EPOLL_CTL_ADD, sockfd, &event
				epoll_wait(epfd, &event, 1, -1);

				// epoll_create(): Creates an epoll instance.
				// struct epoll_event: Structure for epoll events.
				// event.events: Events to monitor.
				// event.data.fd: File descriptor to monitor.
				// epoll_ctl(): Modifies an epoll instance.
				// epoll_wait(): Waits for an event on an epoll instance.
				```
			- **kqueue()**:
				- Monitors multiple file descriptors for I/O readiness.
				- **Timeout**:
					- Specifies the maximum time to wait for an event.
				- **kqueue()** returns the number of ready file descriptors.
				```c
				int kq = kqueue();
				struct kevent event;
				EV_SET(&event, sockfd, EVFILT_READ, EV_ADD, 0, 0, NULL);

				kevent(kq, &event, 1, &event, 1, NULL);

				// kqueue(): Creates a kqueue instance.
				// struct kevent: Structure for kqueue events.
				// EV_SET: Initializes a kevent structure.
				// kevent(): Modifies a kqueue instance.
				```

5. **HTTP Protocol**: A protocol that allows web browsers and servers to communicate over the internet.

	- **Request Methods**:
		- **GET**:
			- Requests data from a server.
		- **POST**:
			- Submits data to a server.
		- **PUT**:
			- Updates data on a server.
		- **DELETE**:
			- Deletes data from a server.
		- **HEAD**:
			- Requests headers from a server.
		- **OPTIONS**:
			- Requests supported methods from a server.
		- **TRACE**:
			- Echoes a request back to a client.
		- **CONNECT**:
			- Converts the request connection to a transparent TCP/IP tunnel.
		
	- **Request Headers**:
		```http
		GET /index.html HTTP/1.1
		Host: www.example.com
		User-Agent: Mozilla/5.0
		Accept: text/html
		Accept-Language: en-US
		Accept-Encoding: gzip, deflate
		Connection: keep-alive
		```
		- **Host**:
			- Specifies the domain name of the server.
		- **User-Agent**:
			- Specifies the user agent making the request.
		- **Accept**:
			- Specifies the media types accepted by the client.
		- **Accept-Language**:
			- Specifies the languages accepted by the client.
		- **Accept-Encoding**:
			- Specifies the encodings accepted by the client.
		- **Connection**:
			- Specifies the connection type.
		- **Content-Type**:
			- Specifies the media type of the request body.
		- **Content-Length**:
			- Specifies the length of the request body.
		- **Cookie**:
			- Specifies the cookies sent by the client.
		- **Authorization**:
			- Specifies the credentials for HTTP authentication.
		- **Cache-Control**:
			- Specifies the caching directives.
		- **If-Modified-Since**:
			- Specifies the date and time of the last modification.
		- **Referer**:
			- Specifies the URL of the previous web page.
		- **Origin**:
			- Specifies the origin of the request.
		- **Upgrade-Insecure-Requests**:
			- Specifies the upgrade request.
		- **X-Requested-With**:
			- Specifies the XMLHttpRequest header.
		- **X-CSRF-Token**:
			- Specifies the CSRF token.
		- **X-Forwarded-For**:
			- Specifies the client IP address.
		- **X-Forwarded-Proto**:
			- Specifies the client protocol.
		- **X-Frame-Options**:
			- Specifies the frame options.
		- **X-XSS-Protection**:
			- Specifies the XSS protection.
		- **X-Content-Type-Options**:
			- Specifies the content type options.
		- **DNT**:
			- Specifies the Do Not Track header.
		- **TE**:
			- Specifies the transfer encoding.
		- **Expect**:
			- Specifies the expected behavior.
	- **Response Headers**:
		```http
		HTTP/1.1 200 OK
		Date: Mon, 01 Jan 2022 00:00:00 GMT
		Server: Apache/2.4.51
		Content-Type: text/html
		Content-Length: 13
		Connection: keep-alive
		Set-Cookie: session=123
		Cache-Control: no-cache
		Last-Modified: Mon, 01 Jan 2022 00:00:00 GMT
		Location: /index.html
		WWW-Authenticate: Basic realm="Restricted"
		Content-Encoding: gzip
		Content-Language: en-US
		Content-Disposition: attachment; filename="example.html"
		Content-Security-Policy: default-src 'self'
		Strict-Transport-Security: max-age=31536000
		X-Content-Type-Options: nosniff
		X-Frame-Options: DENY
		X-XSS-Protection: 1; mode=block
		Referrer-Policy: no-referrer
		Feature-Policy: geolocation 'none'
		Expect-CT: max-age=86400, enforce
		Alt-Svc: h2=":443"; ma=2592000
		Public-Key-Pins: pin-sha256="base64=="; max-age=5184000; includeSubDomains
		Content-Security-Policy-Report-Only: default-src 'self'; report-uri /report
		Expect-CT: max-age=86400, enforce
		Alt-Svc: h2=":443"; ma=2592000
		Public-Key-Pins: pin-sha256="base64=="; max-age=5184000; includeSubDomains
		```
		- **Date**:
			- Specifies the date and time of the response.
		- **Server**:
			- Specifies the server software.
		- **Content-Type**:
			- Specifies the media type of the response body.
		- **Content-Length**:
			- Specifies the length of the response body.
		- **Connection**:
			- Specifies the connection type.
		- **Set-Cookie**:
			- Specifies the cookies sent by the server.
		- **Cache-Control**:
			- Specifies the caching directives.
		- **Last-Modified**:
			- Specifies the date and time of the last modification.
		- **Location**:
			- Specifies the URL for redirection.
		- **WWW-Authenticate**:
			- Specifies the authentication method.
		- **Content-Encoding**:
			- Specifies the encoding of the response body.
		- **Content-Language**:
			- Specifies the language of the response body.
		- **Content-Disposition**:
			- Specifies the disposition of the response body.
		- **Content-Security-Policy**:
			- Specifies the security policy.
		- **Strict-Transport-Security**:
			- Specifies the transport security policy.
		- **X-Content-Type-Options**:
			- Specifies the content type options.
		- **X-Frame-Options**:
			- Specifies the frame options.
		- **X-XSS-Protection**:
			- Specifies the XSS protection.
		- **Referrer-Policy**:
			- Specifies the referrer policy.
		- **Feature-Policy**:
			- Specifies the feature policy.
		- **Expect-CT**:
			- Specifies the Certificate Transparency policy.
		- **Alt-Svc**:
			- Specifies the alternative services.
		- **Public-Key-Pins**:
			- Specifies the public key pins.
		- **Content-Security-Policy-Report-Only**:
			- Specifies the security policy for reporting.
		- **Expect-CT**:
			- Specifies the Certificate Transparency policy.
		- **Alt-Svc**:
			- Specifies the alternative services.
		- **Public-Key-Pins**:
			- Specifies the public key pins.
	- **Status Codes**:
		- **1xx (Informational)**:
			- **100 Continue**:
				- The server has received the request headers and will wait for the request body.
			- **101 Switching Protocols**:
				- The server has agreed to switch protocols.
		- **2xx (Success)**:
			- **200 OK**:
				- The request was successful.
			- **201 Created**:
				- The request has been fulfilled and a new resource has been created.
			- **202 Accepted**:
				- The request has been accepted for processing.
			- **204 No Content**:
				- The server has fulfilled the request but there is no content to send.
		- **3xx (Redirection)**:
			- **301 Moved Permanently**:
				- The requested resource has been permanently moved to a new location.
			- **302 Found**:
				- The requested resource has been temporarily moved to a new location.
			- **304 Not Modified**:
				- The requested resource has not been modified since the last request.
		- **4xx (Client Error)**:
			- **400 Bad Request**:
				- The request could not be understood by the server.
			- **401 Unauthorized**:
				- The request requires user authentication.
			- **403 Forbidden**:
				- The server has refused to fulfill the request.
			- **404 Not Found**:
				- The requested resource could not be found.
		- **5xx (Server Error)**:
			- **500 Internal Server Error**:
				- The server encountered an unexpected condition.
			- **501 Not Implemented**:
				- The server does not support the functionality required to fulfill the request.
			- **502 Bad Gateway**:
				- The server received an invalid response from an upstream server.
			- **503 Service Unavailable**:
				- The server is temporarily unavailable.
	- **HTTP/1.1**:
		- **Persistent Connections**:
			- Allows multiple requests and responses to be sent over a single connection.
		- **Pipelining**:
			- Allows multiple requests to be sent without waiting for the responses.
		- **Chunked Transfer Encoding**:
			- Allows data to be sent in chunks.
		- **Content Negotiation**:
			- Allows the server to send different content based on the client's preferences.
		- **Caching**:
			- Allows the client to store a copy of the response for future use.
		- **Compression**:
			- Allows the server to compress the response before sending it to the client.
		- **Authentication**:
			- Allows the server to require user authentication.
		- **Cookies**:
			- Allows the server to store information on the client's computer.
		- **Redirects**:
			- Allows the server to redirect the client to a different URL.
		- **Error Handling**:
			- Allows the server to send error messages to the client.
		- **Security**:
			- Allows the server to enforce security policies.
	- **HTTP/2**:
		- **Binary Protocol**:
			- Allows data to be sent in binary format.
		- **Multiplexing**:
			- Allows multiple requests and responses to be sent over a single connection.
		- **Header Compression**:
			- Allows headers to be compressed before sending them to the client.
		- **Server Push**:
			- Allows the server to push resources to the client before they are requested.
		- **Stream Prioritization**:
			- Allows the client to prioritize streams.
		- **Flow Control**:
			- Allows the client to control the flow of data.
		- **Security**:
			- Allows the server to enforce security policies.
	- **HTTP/3**:
		- **QUIC Protocol**:
			- Allows data to be sent over UDP.
		- **Multiplexing**:
			- Allows multiple requests and responses to be sent over a single connection.
		- **Header Compression**:
			- Allows headers to be compressed before sending them to the client.
		- **Server Push**:
			- Allows the server to push resources to the client before they are requested.
		- **Stream Prioritization**:
			- Allows the client to prioritize streams.
		- **Flow Control**:
			- Allows the client to control the flow of data.
		- **Security**:
			- Allows the server to enforce security policies.
	- **HTTP/4**:
		- **Future Protocol**:
			- Allows data to be sent over a new protocol.
		- **Multiplexing**:
			- Allows multiple requests and responses to be sent over a single connection.
		- **Header Compression**:
			- Allows headers to be compressed before sending them to the client.
		- **Server Push**:
			- Allows the server to push resources to the client before they are requested.
		- **Stream Prioritization**:
			- Allows the client to prioritize streams.
		- **Flow Control**:
			- Allows the client to control the flow of data.
		- **Security**:
			- Allows the server to enforce security policies.

	- [HTTP Headers - MDN Web Docs](https://developer.mozilla.org/en-US/docs/Web/HTTP/Headers)

6. **Web Server Architecture**: The structure of a web server that handles client requests and serves web pages.

	- **Client**:
		- Sends requests to the server.
	- **Web Server**:
		- Listens for incoming connections.
		- Handles client requests.
		- Serves web pages.
	- **Application Server**:
		- Executes application logic.
		- Communicates with databases.
	- **Database Server**:
		- Stores data.
		- Communicates with application servers.
	- **Reverse Proxy**:
		- Distributes client requests to multiple servers.
		- Improves performance and reliability.
	- **Load Balancer**:
		- Distributes client requests to multiple servers.
		- Improves performance and reliability.
	- **Firewall**:
		- Filters network traffic.
		- Protects against security threats.
	- **Content Delivery Network (CDN)**:
		- Distributes content to multiple servers.
		- Improves performance and reliability.
	- **Domain Name System (DNS)**:
		- Resolves domain names to IP addresses.
		- Routes client requests to servers.
	- **Web Browser**:
		- Requests web pages from servers.
		- Displays web pages to users.






	- [C++ Network Programming Part 1: Sockets](https://youtu.be/gntyAFoZp-E)
	- [Sockets in Operating System](https://youtu.be/uagKTbohimU)
	- [Socket Programming in C/C++ - GeeksforGeeks](https://www.geeksforgeeks.org/socket-programming-cc/)
	- [Socket Programming in C - Tutorialspoint](https://www.tutorialspoint.com/unix_sockets/socket_server_example.htm)
	- [Sockets and Pipes Look Like Files (Unix/fdopen)](https://youtu.be/il4N6KjVQ-s)