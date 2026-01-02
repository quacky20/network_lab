#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>

#define PORT 8000

int main(int argc, char const* argv[]){
    struct sockaddr_in address;
    int sockfd, new_socket;
    int opt = 1;
    socklen_t addrlen = sizeof(address);
    char buffer[1024] = { 0 };
    char* str = "Hello from server";
    ssize_t valread;

    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0){
        perror("connection");
        exit(1);
    }
    
    if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))){
        perror("setsockopt");
        exit(1);
    }
    
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    if (bind(sockfd, (struct sockaddr*)&address, sizeof(address)) < 0){
        perror("bind");
        exit(1);
    }

    if (listen(sockfd, 3) < 0){
        perror("listen");
        exit(1);    
    }

    if ((new_socket = accept(sockfd, (struct sockaddr*)&address, &addrlen)) < 0){
        perror("accept");
        exit(1);
    }
       
    valread = read(new_socket, buffer, 1024 - 1);
    printf("%s\n", buffer);

    send(new_socket, str, strlen(str), 0);
    printf("Message sent");

    close(new_socket);
    close(sockfd);

    return 0;
}
