/* **********************************************
*NAME : SHASHI KANT
*
*SOCKET PROGRAMMING: Proxy Server
*************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <signal.h>

#define BACKLOG 20         // how many pending connections queue will hold


// get sockaddr, IPv4 or IPv6:
void *get_in_addr(struct sockaddr *sa)
{
if (sa->sa_family == AF_INET) {
return &(((struct sockaddr_in*)sa)->sin_addr);
}
return &(((struct sockaddr_in6*)sa)->sin6_addr);
}


int main(int argc, char * argv[])
{
//checks if argument passed through command-line is valid or not
if(argc<2)
error("./proxy <port_no>");

// process id of the child that is created 
pid_t pid;

int sockfd,newsockfd;

struct addrinfo hints, *servinfo, *p;	//It'll return a pointer to a new linked  structures filled out with host name such as its IP address and load up a struct sockaddr

struct sigaction sa;			//responsible for reaping zombie processes that appear as the fork()ed child
struct sockaddr_storage their_addr; 	 // connector's address information
socklen_t sin_size;
int yes=1;				//used for clearing socket if it is in use
char s[INET6_ADDRSTRLEN];		//address  length,,it can hold ipv4 and ipv6 addresses
int rv;

memset(&hints, 0, sizeof hints);                 // make  the struct  empty
hints.ai_family = AF_UNSPEC;                    // don't care IPv4 or IPv6
hints.ai_socktype = SOCK_STREAM;	 //TCP socket stream
hints.ai_flags = AI_PASSIVE;                      // use my IP

if ((rv = getaddrinfo(NULL, argv[1], &hints, &servinfo)) != 0) {
fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
return 1;
}

// loop through all the results and bind to the first we can
for(p = servinfo; p != NULL; p = p->ai_next) 
{

// socket descripter is returned if succecc in initallizing the socket
if ((sockfd = socket(p->ai_family, p->ai_socktype,
p->ai_protocol)) == -1) {
perror("Problem in initializing socket.");
continue;
}
//kills all zombie processes
if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1) 
{
perror("setsockopt");
exit(1);
}
//Associate a socket with an IP address and port number
if (bind(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
close(sockfd);
perror("Problem in Binding.");
continue;
}
break;
}
if (p == NULL) {
fprintf(stderr, "server: failed to bind\n");
return 2;
}
freeaddrinfo(servinfo);        // after the address is filled up in sockaddr, this strucutre is freed

// after successfully binding, the server waits for incomming connection 

if (listen(sockfd, BACKLOG) == -1) {
perror("Problem in listening");
exit(1);
}
// reap all dead processes
sa.sa_handler = sigchld_handler; 
sigemptyset(&sa.sa_mask);
sa.sa_flags = SA_RESTART;
if (sigaction(SIGCHLD, &sa, NULL) == -1) {
perror("sigaction");
exit(1);
}

printf("server: waiting for connections...\n");

accepting:

sin_size = sizeof their_addr;

//Accept an incoming connection on a listening socket and returns 0 if all is well
new_fd = accept(sockfd, (struct sockaddr *)&their_addr, &sin_size);        
if (new_fd == -1) {
perror("Problem in Accepting.");
continue;
}

// converts  string of bits of IP address in numbers-and-dots notation 
inet_ntop(their_addr.ss_family, get_in_addr((struct sockaddr *)&their_addr),s, sizeof s);
printf("server: got connection from %s\n", s);

// creates new child, now socket descriptor obtained will be used to communicate with the proxy server

pid=fork();
if(pid==0)
{

struct sockaddr_in host_addr;
int flag=0,newsockfd1,n,port=0,i,sockfd1;
char buffer[510],t1[300],t2[300],t3[10];
char* temp=NULL;
bzero((char*)buffer,500);
recv(newsockfd,buffer,500,0);
   
sscanf(buffer,"%s %s %s",t1,t2,t3);
 
// proxy server parses the http header and body and converts it in a valid format that can be accpeted by origin server    

if(((strncmp(t1,"GET",3)==0))&&((strncmp(t3,"HTTP/1.1",8)==0)||(strncmp(t3,"HTTP/1.0",8)==0))&&(strncmp(t2,"http://",7)==0))
{
strcpy(t1,t2);
   
flag=0;
   
for(i=7;i<strlen(t2);i++)
{
if(t2[i]==':')
{
flag=1;
break;
}
}
// strtok() used to divide sequence of string in valid tokens   
temp=strtok(t2,"//");
if(flag==0)
{
port=80;
temp=strtok(NULL,"/");
}
else
{
temp=strtok(NULL,":");
}
   
sprintf(t2,"%s",temp);
printf("host = %s",t2);
host=gethostbyname(t2);
   
if(flag==1)
{
temp=strtok(NULL,"/");
port=atoi(temp);
}
   
   
strcat(t1,"^]");
temp=strtok(t1,"//");
temp=strtok(NULL,"/");
if(temp!=NULL)
temp=strtok(NULL,"^]");
printf("\npath = %s\nPort = %d\n",temp,port);
   
// empties the structure  
bzero((char*)&host_addr,sizeof(host_addr));
host_addr.sin_port=htons(port);
host_addr.sin_family=AF_INET;
bcopy((char*)host->h_addr,(char*)&host_addr.sin_addr.s_addr,host->h_length);
   
sockfd1=socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
newsockfd1=connect(sockfd1,(struct sockaddr*)&host_addr,sizeof(struct sockaddr));
sprintf(buffer,"\nConnected to %s  IP - %s\n",t2,inet_ntoa(host_addr.sin_addr));
if(newsockfd1<0)
error("Error in connecting to remote server");
   
printf("\n%s\n",buffer);
//send(newsockfd,buffer,strlen(buffer),0);
bzero((char*)buffer,sizeof(buffer));
if(temp!=NULL)
sprintf(buffer,"GET /%s %s\r\nHost: %s\r\nConnection: close\r\n\r\n",temp,t3,t2);
else
sprintf(buffer,"GET / %s\r\nHost: %s\r\nConnection: close\r\n\r\n",t3,t2);
 
 //origin server sends the data 
n=send(sockfd1,buffer,strlen(buffer),0);
printf("\n%s\n",buffer);
if(n<0)
error("Error writing to socket");
else{
do
{
bzero((char*)buffer,500);
n=recv(sockfd1,buffer,500,0);
if(!(n<=0))
send(newsockfd,buffer,n,0);
}while(n>0);
}
}
else
{
send(newsockfd,"400 : BAD REQUEST\nONLY HTTP REQUESTS ALLOWED",18,0);
}

//closes a partiulcar socket connection once response has been send
close(sockfd1);
close(newsockfd);
close(sockfd);
_exit(0);
}
else
{
close(newsockfd);
goto accepting;
}
return 0;
}

























































