#include<arpa/inet.h>
#include<unistd.h>
#include<stdio.h>
#include<stdlib.h>
#define SA struct sockaddr
#define PORT 8862

int main(int argc,char**argv)
{
	int sockfd;
	char fname[25];
	int len;
	struct sockaddr_in servaddr,cliaddr;
	sockfd=socket(AF_INET,SOCK_STREAM,0);
	if(sockfd < 0){
		perror("socket creation failed");
		exit(EXIT_FAILURE);
	}

	bzero(&servaddr,sizeof(servaddr));
	
	servaddr.sin_family=AF_INET;
	servaddr.sin_addr.s_addr=htonl(INADDR_ANY);
	servaddr.sin_port=htons(PORT);
	
	inet_pton(AF_INET,"127.0.0.1",&servaddr.sin_addr);
	if(connect(sockfd,(SA*)&servaddr,sizeof(servaddr)) < 0){
		perror("connection failed");
		exit(EXIT_FAILURE);
	}

	// The connect() system call connects the socket referred to by the
	// file descriptor sockfd to the address specified by addr.
	// Server’s address and port is specified in servaddr.

	char buffer[100];
	printf("Enter filename you want to send : %s\n", buffer);
	scanf("%s", buffer);
	write(sockfd, buffer, 100);


	FILE *fp;
	fp = fopen("input.txt","r");

	while(1){
	    char buff[100]={0};
	    int chars_read = fread(buff,1,100,fp);  
	    if(chars_read > 0){
	    	// printf("Sending \n");
	        write(sockfd, buff, chars_read);
	    }
	    if (chars_read < 100)
	    {
	    	if (feof(fp))
	            printf("End of file\n");
	        if (ferror(fp))
	        	printf("Error reading\n");
	        break;
	    }
	}

	printf("the file was sent successfully");
	return 0;
}