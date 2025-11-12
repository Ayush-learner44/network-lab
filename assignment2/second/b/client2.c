#include <stdio.h>     // printf()
#include <stdlib.h>    // exit()
#include <string.h>    // memset()
#include <unistd.h>    // close()
#include <arpa/inet.h> // socket functions & sockaddr_in

#define CLIENT2_PORT 9090

struct Data
{
    char c;
    int i;
    float f;
};

int main()
{
    int sockfd;
    struct sockaddr_in cliaddr;
    struct Data d;

    // 1️⃣ Create UDP socket
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);

    // 2️⃣ Fill this client's address
    memset(&cliaddr, 0, sizeof(cliaddr));
    cliaddr.sin_family = AF_INET;
    cliaddr.sin_addr.s_addr = INADDR_ANY;
    cliaddr.sin_port = htons(CLIENT2_PORT);

    // 3️⃣ Bind socket to this address
    bind(sockfd, (struct sockaddr *)&cliaddr, sizeof(cliaddr));

    printf("Client2: waiting for struct from server...\n");

    // 4️⃣ Receive struct from server
    recvfrom(sockfd, &d, sizeof(d), 0, NULL, NULL);
    printf("Client2: got c='%c', i=%d, f=%.2f\n", d.c, d.i, d.f);

    // 5️⃣ Close socket
    close(sockfd);
    return 0;
}