#include <stdio.h>     // printf()
#include <string.h>    // memset()
#include <unistd.h>    // close()
#include <arpa/inet.h> // socket(), connect(), recv()

#define PORT2 9090 // Must match server's PORT2

int main()
{
    // Create TCP socket
    int s = socket(AF_INET, SOCK_STREAM, 0);

    // Prepare server address
    struct sockaddr_in a;
    memset(&a, 0, sizeof(a));
    a.sin_family = AF_INET;
    a.sin_port = htons(PORT2);
    inet_pton(AF_INET, "127.0.0.1", &a.sin_addr); // Server IP

    // Connect to server
    connect(s, (struct sockaddr *)&a, sizeof(a));

    // Receive char from server
    char ch;
    recv(s, &ch, 1, 0);
    printf("Client2: got '%c' from server\n", ch);

    // Close socket
    close(s);
    return 0;
}