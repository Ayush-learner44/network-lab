#include <stdio.h>     // printf(), scanf()
#include <string.h>    // memset()
#include <unistd.h>    // close()
#include <arpa/inet.h> // socket(), connect(), send()

#define PORT1 8080 // Must match server's PORT1

// Define the same structure
struct Data
{
    char c;
    int i;
    float f;
};

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

    // Get struct data from user
    struct Data d;
    printf("Enter char: ");
    scanf(" %c", &d.c);
    printf("Enter int: ");
    scanf("%d", &d.i);
    printf("Enter float: ");
    scanf("%f", &d.f);

    // Send struct to server
    send(s, &d, sizeof(d), 0);
    printf("Client1: sent struct to server\n");

    // Close socket
    close(s);
    return 0;
}