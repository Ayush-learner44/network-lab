#include <stdio.h>     // printf(), scanf()
#include <stdlib.h>    // exit()
#include <string.h>    // memset()
#include <unistd.h>    // close()
#include <arpa/inet.h> // socket functions & sockaddr_in

#define SERVER_PORT 8080

struct Data
{
    char c;
    int i;
    float f;
};

int main()
{
    int sockfd;
    struct sockaddr_in servaddr;
    struct Data d;

    // 1️⃣ Create UDP socket
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);

    // 2️⃣ Fill server address
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(SERVER_PORT);
    inet_pton(AF_INET, "127.0.0.1", &servaddr.sin_addr); // change to server's LAN IP

    // 3️⃣ Get struct data from user
    printf("Enter char: ");
    scanf(" %c", &d.c);
    printf("Enter int: ");
    scanf("%d", &d.i);
    printf("Enter float: ");
    scanf("%f", &d.f);

    // 4️⃣ Send struct to server
    sendto(sockfd, &d, sizeof(d), 0, (struct sockaddr *)&servaddr, sizeof(servaddr));
    printf("Client1: sent struct to server\n");

    // 5️⃣ Close socket
    close(sockfd);
    return 0;
}