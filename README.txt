Description:

 The objective of the  proxy server is to act as an intermediary  in order to 'forward' TCP based connections from external clients
 onto a singular remote server. Once a request is send through terminal or browser, it first has to pass through this proxy server, then it sends to 
 remote or origin server after processing. Finally, response message  passes through proxy server to client that has requsested for the page. 
 
 
How to run:
Copy all the files in one directory.
1)Open two different terminals.
	In the terminal:
	$ cd /path/to/code/
2)Run the source code in one of the terminal as:
	gcc -o proxy proxy.c
3)Now run the proxy server on same terminal as:
	./proxy <port number>
4)In other terminal telnet the address:			 //localhost i.e. the host which has our proxy server,in this case my system
	telnet localhost <port number>			//same port number         
 The headers and HTML of the requested homepage  will be displayed on the terminal screen.
 
 e.g.
 In the first terminal
 ./proxy 12000
 
 in the second terminal
 telnet localhost 12000
 
Trying 127.0.0.1...
Connected to localhost.
Escape character is '^]'.
GET http://www.iitg.ernet.in/ HTTP/1.0

 after entering,, page appears on the terminal used for telnetting
 
 
 Limitation:

1)Unable to parse multi-line http request.
e.g. 
GET / HTTP/1.0
Host: www.iitg.ernet.in

2)only GET Method Implemented, Using other Method will throw exception like ERROR 400: BAD REQUEST .
 
Code Description:

socket(): 
int socket(int domain, int type, int protocol);
Get the File Descriptor

bind():
int bind(int sockfd, struct sockaddr *my_addr, int addrlen);
associate a socket with a port the host

listen():
int listen(int sockfd, int backlog);
backlog- maximum number of request that can be in a queue on proxy server
wait for incoming connections

So for listening for incoming connections, the sequence of system calls are as follows:

sockaddr_in();
socket();
bind();
listen();

accept():
int accept(int sockfd, struct sockaddr *addr, socklen_t *addrlen);   //sockfd is the listen()ing socket descriptor.
accept an incomming connection.

send():
int send(int sockfd, const void *msg, int len, int flags);     //sockfd is the socket descriptor you want to send data to
 sending messages to client or server.
 
 
 recv()
int recv(int sockfd, const void *msg, int len, int flags);
receiving messages from server.

close():
close(sockfd);
This will prevent any more reads and writes to the socket.

















































