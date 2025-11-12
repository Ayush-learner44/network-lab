#include <stdio.h>     // printf(), perror()
#include <stdlib.h>    // exit(), rand()
#include <string.h>    // memset()
#include <unistd.h>    // close()
#include <arpa/inet.h> // socket functions & sockaddr_in

#define SERVER_PORT 8080  // Port to receive from Client1
#define CLIENT2_PORT 9090 // Port to send to Client2

// Structure to send/receive
struct Data
{
    char c;
    int i;
    float f;
};

int main()
{
    int sockfd; // socket file descriptor
    struct sockaddr_in servaddr, cliaddr2;
    struct Data d; // struct buffer

    // 1️⃣ Create UDP socket: AF_INET = IPv4, SOCK_DGRAM = UDP
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);

    // 2️⃣ Prepare server address
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = INADDR_ANY;
    servaddr.sin_port = htons(SERVER_PORT);

    // 3️⃣ Bind socket to server address
    bind(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr));

    printf("Server: waiting for struct from Client1...\n");

    // 4️⃣ Receive struct from Client1
    recvfrom(sockfd, &d, sizeof(d), 0, NULL, NULL);
    printf("Server: got c='%c', i=%d, f=%.2f\n", d.c, d.i, d.f);

    // 5️⃣ Modify struct with random values (no time seed)
    d.c = 'A' + (rand() % 26);           // random uppercase letter
    d.i = rand() % 1000;                 // random int 0–999
    d.f = (float)(rand() % 1000) / 10.0; // random float 0.0–99.9

    // 6️⃣ Prepare Client2 address
    memset(&cliaddr2, 0, sizeof(cliaddr2));
    cliaddr2.sin_family = AF_INET;
    cliaddr2.sin_port = htons(CLIENT2_PORT);
    inet_pton(AF_INET, "127.0.0.1", &cliaddr2.sin_addr); // change to LAN IP for multi-PC

    // 7️⃣ Send modified struct to Client2
    sendto(sockfd, &d, sizeof(d), 0, (struct sockaddr *)&cliaddr2, sizeof(cliaddr2));
    printf("Server: sent random struct to Client2\n");

    // 8️⃣ Close socket
    close(sockfd);
    return 0;
}