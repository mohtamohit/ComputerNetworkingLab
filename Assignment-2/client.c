#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

int main(int argc , char *argv[]){
    int socket_desc;
    struct sockaddr_in server;
    char message_to_send[2000];
    char server_reply[2000];

    socket_desc = socket(AF_INET , SOCK_STREAM , 0);
    if (socket_desc == -1){
        printf("Could not create socket.");
    }
    server.sin_addr.s_addr = inet_addr("10.100.43.69");
    server.sin_family = AF_INET;
    server.sin_port = htons(8883);
    if(connect(socket_desc, (struct sockaddr*) &server , sizeof(server)) < 0){
        puts("Connection failed.");
        return 1;
    }

    bzero(message_to_send,100);
    printf("C : ");
    fgets(message_to_send, sizeof(message_to_send), stdin);
        
    if(send(socket_desc , message_to_send , strlen(message_to_send) , 0) < 0)
      	puts("Sending Failed.");

    bzero(server_reply, 2000);
    read(socket_desc, server_reply , 2000);
    printf("S : ");
    puts(server_reply);
    return 0;
}
