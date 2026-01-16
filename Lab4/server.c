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
#include <fcntl.h>
#include <sys/stat.h>
#include <time.h>

#define PORT "8000"

#define CHUNKSIZE 4096

#define BACKLOG 10

void *get_in_addr(struct sockaddr *sa){
    if (sa->sa_family == AF_INET){
        return &(((struct sockaddr_in*)sa)->sin_addr);    
    }
    return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

int main(void){
    int sockfd, new_fd;
    struct addrinfo hints, *servinfo, *p;
    struct sockaddr_storage client_add;
    socklen_t sin_size;
    int yes = 1;
    char s[INET6_ADDRSTRLEN];
    int rv;

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    if ((rv = getaddrinfo(NULL, PORT, &hints, &servinfo)) != 0){
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
        return 1;
    }

    for (p = servinfo; p!=NULL; p=p->ai_next){
        if ((sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1){
            perror("server: socket");
            continue;        
        }    
        if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1){
            perror("setsockopt");
        }
        if ((bind(sockfd, p->ai_addr, p->ai_addrlen)) == -1){
            perror("server: bind");
            continue;        
        }
        break;
    }
    
    freeaddrinfo(servinfo);

    if (p == NULL){
        fprintf(stderr, "server: failed to bind");
        exit(1);
    }

    if (listen(sockfd, BACKLOG) == -1){
        perror("listen");
        exit(1);
    }
    
    while(1){
        sin_size = sizeof client_add;
        new_fd = accept(sockfd, (struct sockaddr*)&client_add, &sin_size);
        if (new_fd == -1){
            perror("accept");
            continue;        
        }    

        time_t t = time(NULL);
        char current_time[80];
        struct tm *currentTime = localtime(&t);
        strftime(current_time, sizeof current_time, "%Y-%m-%d-%H:%M:%S", currentTime);
        
        inet_ntop(client_add.ss_family, get_in_addr((struct sockaddr*)&client_add), s, sizeof(s));

        if (!fork()){
            close(sockfd);
            
            t = time(NULL);
            currentTime = localtime(&t);
            strftime(current_time, sizeof current_time, "%Y-%m-%d-%H:%M:%S", currentTime);

            uint32_t net_name_len;
            recv(new_fd, &net_name_len, sizeof(net_name_len), MSG_WAITALL);
            uint32_t name_len = ntohl(net_name_len);
            char name[name_len + 1];
            recv(new_fd, name, name_len, MSG_WAITALL);
            name[name_len] = '\0';

            printf("<%s> got connection from %s (%s)\n", current_time, name, s);

            uint32_t net_len;
            recv(new_fd, &net_len, sizeof(net_len), MSG_WAITALL);
            uint32_t filename_len = ntohl(net_len);
            char filename[filename_len + 1];
            recv(new_fd, filename, filename_len, MSG_WAITALL);
            filename[filename_len] = '\0';

            char path[256] = "recieved/";
            snprintf(path, sizeof(path), "recieved/%s", filename);

            int out_fd = open(path, O_WRONLY | O_CREAT | O_TRUNC, 0644);
            if (out_fd == -1) {
                perror("open");
                exit(1);
            }

            char buffer[CHUNKSIZE];
            ssize_t bytes_recv;      

            ssize_t total_written = 0;
            while ((bytes_recv = read(new_fd, buffer, CHUNKSIZE)) > 0) {
                while (total_written < bytes_recv) {
                    ssize_t written = write(out_fd, buffer + total_written, bytes_recv - total_written);
                    if (written == -1) {
                        perror("write");
                        close(out_fd);
                        exit(1);
                    }
                    total_written += written;
                }
            }
            printf("<%s> recieved file (%s) of size %ld bytes from %s \n", current_time, filename, total_written, s);

            if (bytes_recv == -1) {
                perror("recv");
            }

            close(new_fd);
            close(out_fd);
            exit(0);
        }
        close(new_fd);
    }
    return 0;
}

