#include <stdio.h>     // printf(), scanf()
#include <stdlib.h>    // exit()
#include <string.h>    // memset()
#include <unistd.h>    // close()
#include <arpa/inet.h> // socket functions & sockaddr_in

#define SERVER_PORT 8080 // Port where server is listening

int main()
{
    int sockfd;                  // socket file descriptor (ID for the socket)
    struct sockaddr_in servaddr; // server address structure
    char ch;                     // character to send

    // 1️⃣ Create UDP socket: AF_INET = IPv4, SOCK_DGRAM = UDP
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);

    // 2️⃣ Fill server address
    memset(&servaddr, 0, sizeof(servaddr));              // clear structure
    servaddr.sin_family = AF_INET;                       // IPv4
    servaddr.sin_port = htons(SERVER_PORT);              // server port in network byte order
    inet_pton(AF_INET, "127.0.0.1", &servaddr.sin_addr); // server IP (change for LAN)

    // 3️⃣ Get char from user
    printf("Client1: enter a character: ");
    scanf(" %c", &ch);

    // 4️⃣ Send char to server
    sendto(sockfd, &ch, sizeof(ch), 0, (struct sockaddr *)&servaddr, sizeof(servaddr));
    printf("Client1: sent '%c' to server\n", ch);

    // 5️⃣ Close socket
    close(sockfd);
    return 0;
}