#include <stdio.h>     // printf()
#include <string.h>    // memset()
#include <unistd.h>    // close()
#include <arpa/inet.h> // socket(), bind(), listen(), accept(), send(), recv()

#define PORT1 8080 // Port where Client1 will connect
#define PORT2 9090 // Port where Client2 will connect

int main()
{
    // Create two TCP listening sockets: one for Client1, one for Client2
    // AF_INET = IPv4, SOCK_STREAM = TCP, 0 = default protocol
    int s1 = socket(AF_INET, SOCK_STREAM, 0);
    int s2 = socket(AF_INET, SOCK_STREAM, 0);

    // Address structures for each listening socket
    struct sockaddr_in a1, a2;
    memset(&a1, 0, sizeof(a1));
    memset(&a2, 0, sizeof(a2));

    // Fill address for Client1 listener
    a1.sin_family = AF_INET;         // IPv4
    a1.sin_port = htons(PORT1);      // Port in network byte order
    a1.sin_addr.s_addr = INADDR_ANY; // Listen on all local IPs

    // Fill address for Client2 listener
    a2.sin_family = AF_INET;
    a2.sin_port = htons(PORT2);
    a2.sin_addr.s_addr = INADDR_ANY;

    // Bind sockets to their addresses
    bind(s1, (struct sockaddr *)&a1, sizeof(a1));
    bind(s2, (struct sockaddr *)&a2, sizeof(a2));

    // Listen for incoming connections (max 1 queued)
    listen(s1, 1);
    listen(s2, 1);

    printf("Server: waiting for Client1...\n");
    int c1 = accept(s1, NULL, NULL); // Accept connection from Client1

    printf("Server: waiting for Client2...\n");
    int c2 = accept(s2, NULL, NULL); // Accept connection from Client2

    // Receive a single char from Client1
    char ch;
    recv(c1, &ch, 1, 0);
    printf("Server: got '%c' from Client1\n", ch);

    // Decrement the char (e.g., 'd' → 'c')
    ch = ch - 1;

    // Send the modified char to Client2
    send(c2, &ch, 1, 0);
    printf("Server: sent '%c' to Client2\n", ch);

    // Close all sockets
    close(c1);
    close(c2);
    close(s1);
    close(s2);
    return 0;
}