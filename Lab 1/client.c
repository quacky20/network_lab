#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#define PORT 8000

int main(int argc, char const* argv[]){
    int status, valread, sockfd;
    struct sockaddr_in serv_addr;
    char* hello = "Hello from client";
    char buffer[1024] = { 0 };

    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0){
        perror("Socket creation failed");
        return -1;
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);

    if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0){
        perror("Invalid address");
        return -1;    
    }

    if ((status = connect(sockfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr))) < 0){
        perror("Connection failed");
        return -1;  
    }

    send(sockfd, hello, strlen(hello), 0);
    printf("Hello message sent \n");
    valread = read(sockfd, buffer, 1024 - 1);
    printf("%s\n", buffer);
    close(sockfd);

    return 0;
}

