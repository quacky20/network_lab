#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>

#define PORT 8081
#define MAXLINE 1000

char* printBinary(char c, char res[]) {
    int len = strlen(res); 

    for (int i = 7; i >= 0; i--) {
        res[len++] = ((c >> i) & 1) + '0';
    }

    res[len] = '\0';
    return res;
}

int main() {
    char buffer[1000];
    int listenfd, len;
    struct sockaddr_in servaddr, cliaddr;
    bzero(&servaddr, sizeof(servaddr));

    listenfd = socket(AF_INET, SOCK_DGRAM, 0);
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(PORT);
    servaddr.sin_family = AF_INET;

    bind(listenfd, (struct sockaddr*)&servaddr, sizeof(servaddr));

    len = sizeof(cliaddr);
    int n = recvfrom(listenfd, buffer, sizeof(buffer), 0,
                     (struct sockaddr*)&cliaddr, &len);
    buffer[n] = '\0';

    char message[1000] = "Acknowledged: ";
    strcat(message, buffer);
    puts(message);

    char result[100];
    result[0] = '\0';

    for (int i = 0; i < 5 && buffer[i] != '\0'; i++) {
        printBinary(buffer[i], result);
    }

    sendto(listenfd, result, strlen(result), 0,
           (struct sockaddr*)&cliaddr, sizeof(cliaddr));
}

