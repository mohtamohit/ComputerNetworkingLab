#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <time.h>

int main(){
	int socket_desc, c;
	int new_socket;
	time_t t = time(NULL);
	struct tm *tm = localtime(&t);
	
	// receive socket_desc of client
	struct sockaddr_in server, client;

	char message_to_client[64];
	char message_from_client[64];

	// create a socket
	socket_desc = socket(AF_INET, SOCK_STREAM, 0);
	if(socket_desc == -1){
		puts("Failed to create socket");
	}

	// prepare server information
	server.sin_family = AF_INET;

	// SERVER IPs
	server.sin_addr.s_addr = INADDR_ANY;
	
	// port number to listen
	server.sin_port = htons(8883); 			

	// bind socket_desc with server info
	if(bind(socket_desc,(struct sockaddr *)&server , sizeof(server)) < 0)
	{
		puts("Bind failed.");
		return 0;
	}
	puts("Successfully binded. Creating Chat ..");

	// set listen to socket: socket_desc
	listen(socket_desc, 3);					
	
	c = sizeof(struct sockaddr_in);
	while(1){
		new_socket = accept(socket_desc, (struct sockaddr*)&client, (socklen_t*)&c);
		bzero(message_from_client,100);
		read(new_socket,message_from_client,sizeof(message_from_client));
		if(new_socket < 0){
			puts("ACCEPT FAILED");
			continue;
		}
		printf("Client: ");
		puts(message_from_client);
		strftime(message_to_client, sizeof(message_to_client), "%c", tm);
		printf("Server: ");
		printf("%s\n", message_to_client);
		//fgets(message_to_client, sizeof(message_to_client), stdin);
			
		write(new_socket, message_to_client, strlen(message_to_client));
	}
	return 0;
}
