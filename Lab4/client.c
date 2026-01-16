#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <fcntl.h>

#define PORT "8000"

#define CHUNKSIZE 4096

#define MAXDATASIZE 100

void *get_in_addr(struct sockaddr *sa){
    if (sa->sa_family == AF_INET){
        return &(((struct sockaddr_in*)sa)->sin_addr);    
    }
    return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

int main(int argc, char* argv[]){
    int sockfd, numbytes;
    char buf[MAXDATASIZE];
    struct addrinfo hints, *servinfo, *p;
    char s[INET6_ADDRSTRLEN];
    int rv;

    if (argc != 4){
        fprintf(stderr,"usage: client hostname filename client_name\n");
        exit(1);    
    }

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;

    if ((rv = getaddrinfo(argv[1], PORT, &hints, &servinfo)) != 0){
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
        return 1;
    }

    for (p = servinfo; p!=NULL; p=p->ai_next){
        if ((sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1){
            perror("client: socket");
            continue;        
        }    
        
        inet_ntop(p->ai_family, get_in_addr((struct sockaddr*)p->ai_addr), s, sizeof(s));
        printf("client: attempting to connect to %s\n", s);

        if (connect(sockfd, p->ai_addr, p->ai_addrlen) == -1){
            perror("client: connect");
            close(sockfd);
            continue;        
        }
        break;
    }

    if (p == NULL){
        fprintf(stderr, "client: failed to connect");
        exit(2);
    }

    inet_ntop(p->ai_family, get_in_addr((struct sockaddr*)p->ai_addr), s, sizeof(s));
    printf("client: connected to %s\n", s);

    freeaddrinfo(servinfo);

    int fd = open(argv[2], O_RDONLY);
    if(fd == -1){
        perror("open");
        close(sockfd);
        exit(1);            
    }

    uint32_t name_len = strlen(argv[3]);
    uint32_t net_name_len = htonl(name_len);    

    send(sockfd, &net_name_len, sizeof net_name_len, 0);
    send(sockfd, argv[3], name_len, 0);
    
    uint32_t filename_len = strlen(argv[2]);
    uint32_t net_len = htonl(filename_len);    

    send(sockfd, &net_len, sizeof net_len, 0);
    send(sockfd, argv[2], filename_len, 0);

    char buffer[CHUNKSIZE];
    ssize_t bytes_read;

    while((bytes_read = read(fd, buffer, CHUNKSIZE)) > 0){
        size_t total_sent = 0;
        while (total_sent < bytes_read){
            ssize_t sent = send(sockfd, buffer + total_sent, bytes_read - total_sent, 0);
            if (sent == -1){
                perror("send");
                close(fd);
                close(sockfd);
                exit(1);                    
            }
            total_sent += sent;
        }            
    }            

    if (bytes_read == -1){
        perror("read");            
    }  

    close(fd);

    close(sockfd);

    return 0;
}
