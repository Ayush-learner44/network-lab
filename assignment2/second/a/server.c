#include <stdio.h>     // printf(), perror()
#include <stdlib.h>    // exit()
#include <string.h>    // memset()
#include <unistd.h>    // close()
#include <arpa/inet.h> // socket functions & sockaddr_in

#define SERVER_PORT 8080  // Port to receive from Client1
#define CLIENT2_PORT 9090 // Port to send to Client2

int main()
{
    int sockfd; // socket file descriptor
    struct sockaddr_in servaddr, cliaddr2;
    char ch; // single character buffer

    // 1️⃣ Create UDP socket: AF_INET = IPv4, SOCK_DGRAM = UDP
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);

    // 2️⃣ Prepare server address (where this server will listen)
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;          // IPv4
    servaddr.sin_addr.s_addr = INADDR_ANY;  // listen on any local IP
    servaddr.sin_port = htons(SERVER_PORT); // convert port to network byte order

    // 3️⃣ Bind socket to the server address
    bind(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr));

    printf("Server: waiting for char from Client1...\n");

    // 4️⃣ Receive 1 byte from Client1
    recvfrom(sockfd, &ch, sizeof(ch), 0, NULL, NULL);
    printf("Server: got '%c'\n", ch);

    // 5️⃣ Decrement the character
    ch = ch - 1;

    // 6️⃣ Prepare Client2's address
    memset(&cliaddr2, 0, sizeof(cliaddr2));
    cliaddr2.sin_family = AF_INET;
    cliaddr2.sin_port = htons(CLIENT2_PORT);
    inet_pton(AF_INET, "127.0.0.1", &cliaddr2.sin_addr); // change to LAN IP for multi-PC

    // 7️⃣ Send modified char to Client2
    sendto(sockfd, &ch, sizeof(ch), 0, (struct sockaddr *)&cliaddr2, sizeof(cliaddr2));
    printf("Server: sent '%c' to Client2\n", ch);

    // 8️⃣ Close the socket
    close(sockfd);
    return 0;
}