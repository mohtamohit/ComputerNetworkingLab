#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h>
  
int main(int argc, char const *argv[])
{
    struct sockaddr_in address;
    int sock = 0, valread;
    struct sockaddr_in serv_addr;

    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        printf("\n Socket creation error \n");
        return -1;
    }
  
    memset(&serv_addr, '0', sizeof(serv_addr));
  
    serv_addr.sin_family = AF_INET;

    for(int i = 0; i<=1024; i++){
        serv_addr.sin_port = htons(i);      
        // Convert IPv4 and IPv6 addresses from text to binary form
        if(inet_pton(AF_INET, "10.100.76.78", &serv_addr.sin_addr)<=0) 
        {
            printf("\nInvalid address/ Address not supported \n");
            continue;
        }
      
        if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) >= 0)
        {
            printf("%d", i);
        }
    }
    return 0;
}
