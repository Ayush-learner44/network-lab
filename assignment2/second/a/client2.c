#include <stdio.h>     // printf()
#include <stdlib.h>    // exit()
#include <string.h>    // memset()
#include <unistd.h>    // close()
#include <arpa/inet.h> // socket functions & sockaddr_in

#define CLIENT2_PORT 9090 // Port where this client will receive data

int main()
{
    int sockfd;                 // socket file descriptor
    struct sockaddr_in cliaddr; // this client's address
    char ch;                    // received character

    // 1️⃣ Create UDP socket
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);

    // 2️⃣ Fill this client's address
    memset(&cliaddr, 0, sizeof(cliaddr));
    cliaddr.sin_family = AF_INET;           // IPv4
    cliaddr.sin_addr.s_addr = INADDR_ANY;   // accept data on any local IP
    cliaddr.sin_port = htons(CLIENT2_PORT); // port to listen on

    // 3️⃣ Bind socket to this address
    bind(sockfd, (struct sockaddr *)&cliaddr, sizeof(cliaddr));

    printf("Client2: waiting for char from server...\n");

    // 4️⃣ Receive char from server
    recvfrom(sockfd, &ch, sizeof(ch), 0, NULL, NULL);
    printf("Client2: received '%c' from server\n", ch);

    // 5️⃣ Close socket
    close(sockfd);
    return 0;
}