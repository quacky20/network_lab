#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <stdlib.h>

#define PORT 8081
#define MAXLINE 1000

int main(){
    char buffer[1000];
    char *message = "This is a test message";
    int sockfd, n;
    struct sockaddr_in servaddr;
    bzero(&servaddr, sizeof(servaddr));

    servaddr.sin_addr.s_addr = inet_addr("127.0.0.1");
    servaddr.sin_port = htons(PORT);
    servaddr.sin_family = AF_INET;

    sockfd = socket(AF_INET, SOCK_DGRAM, 0);

    if (connect(sockfd, (struct sockaddr*)&servaddr, sizeof(servaddr))<0){
        printf("Conection error");
        exit(0);
    }

    sendto(sockfd, message, strlen(message), 0, (struct sockaddr*)NULL, sizeof(servaddr));

    n = recvfrom(sockfd, buffer, sizeof(buffer), 0, (struct sockaddr*)NULL, NULL);
    buffer[n] = '\0';
    puts(buffer);    

    close(sockfd);
}
