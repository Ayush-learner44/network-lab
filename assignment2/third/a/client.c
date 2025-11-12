#include <stdio.h>     // printf(), scanf()
#include <string.h>    // memset()
#include <unistd.h>    // close()
#include <arpa/inet.h> // socket(), connect(), send()

#define PORT1 8080 // Must match server's PORT1

int main()
{
    // Create TCP socket
    int s = socket(AF_INET, SOCK_STREAM, 0);

    // Prepare server address
    struct sockaddr_in a;
    memset(&a, 0, sizeof(a));
    a.sin_family = AF_INET;
    a.sin_port = htons(PORT1);
    inet_pton(AF_INET, "127.0.0.1", &a.sin_addr); // Server IP

    // Connect to server
    connect(s, (struct sockaddr *)&a, sizeof(a));

    // Get char from user
    char ch;
    printf("Client1: enter a character: ");
    scanf(" %c", &ch);

    // Send char to server
    send(s, &ch, 1, 0);
    printf("Client1: sent '%c' to server\n", ch);

    // Close socket
    close(s);
    return 0;
}