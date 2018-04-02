#include<stdio.h>
#include<sys/types.h>
#include<string.h>
#include<stdlib.h>
#include<sys/socket.h>
#include<arpa/inet.h>
#include<unistd.h>
#define SA struct sockaddr
#define LISTENQ 5
#define PORT 8862

int main(int argc,char**argv)
{
	int listenfd,connfd;
	socklen_t client;
	struct sockaddr_in servaddr,cliaddr;
	
	listenfd = socket(AF_INET,SOCK_STREAM,0);
	// Here we have created a socket descriptor, an integer like a file handle
	// Syntax : int sockfd = socket(communication domain, here IPv4 protocol, communication type - stream for tcp and dgram for udp,
	// protocol value for Internet Protocol (IP) is 0)
	
	if(listenfd < 0){
		perror("socket creation failed");
		exit(EXIT_FAILURE);
	}

	bzero(&servaddr,sizeof(servaddr));
	// The bzero() function shall place n zero-valued bytes in the area pointed to by s.
	// servaddr contains the address of the server

	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);	
	servaddr.sin_port = htons(PORT);
	
	// Attaching the socker to the port; the bind function is binding the socket to adress and pot number specified in serveraddr
	bind(listenfd,(SA*)&servaddr,sizeof(servaddr));

	// Now with Listen we make the socket wait for client to approach, LISTENQ is the backlog
	// ie. max len for which the queue of pending connections for listenfd may grow
	listen(listenfd,LISTENQ);
	
	client=sizeof(cliaddr);
	connfd = accept(listenfd,(SA*)&cliaddr,&client);
	// We extract the first connection request for the listening socket, sockfd, creates a new connected socket and returns
	// the file descriptor for the newly created socket

	char buffer[100];
	FILE *fp;
	read(connfd,buffer,100);
	fp = fopen(buffer,"a");
	
	int chars_recvd = 0;
	char file_content[100] = {0};
	while((chars_recvd = read(connfd, file_content, 100)) > 0){
		// printf("getting the file %d", chars_recvd);
		
		fprintf(fp,"%s",file_content);

		// file_content[100] = {0};
	}
	
	printf("the file was received successfully");
}