#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>

#define MAXLINE 4096
#define PORT 8080

// Global variables needed for the signal handler
int sockfd;
struct sockaddr_in cliaddr;
socklen_t len;
int pd[2]; // The pipe descriptors

// Signal Handler: Reads from pipe and sends to client
void piperead(int sig)
{
    char buf[MAXLINE];
    int n;

    // Read from pipe (non-blocking) and send to client
    while ((n = read(pd[0], buf, MAXLINE - 1)) > 0)
    {
        buf[n] = '\0';
        sendto(sockfd, buf, n, 0, (struct sockaddr *)&cliaddr, len);
    }
}

int main()
{
    struct sockaddr_in servaddr;
    char buffer[MAXLINE];

    // 1. Create UDP Socket
    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
    {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    memset(&servaddr, 0, sizeof(servaddr));
    memset(&cliaddr, 0, sizeof(cliaddr));

    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = INADDR_ANY;
    servaddr.sin_port = htons(PORT);

    // 2. Bind the socket
    if (bind(sockfd, (const struct sockaddr *)&servaddr, sizeof(servaddr)) < 0)
    {
        perror("Bind failed");
        exit(EXIT_FAILURE);
    }

    printf("UDP Server listening on port %d...\n", PORT);

    // 3. Setup the Pipe
    if (pipe(pd) < 0)
    {
        perror("Pipe failed");
        exit(EXIT_FAILURE);
    }

    // 4. Redirect stdout to the pipe
    close(STDOUT_FILENO); // Close terminal output
    if (dup(pd[1]) != STDOUT_FILENO)
    { // Replace stdout with pipe-write
        perror("Dup failed");
        exit(EXIT_FAILURE);
    }
    close(pd[1]); // Close original write-end (we have the copy on fd 1 now)

    // 5. Setup Asynchronous I/O on Read-End
    // This tells the OS: "Send me a SIGIO signal when data is in the pipe"
    if (fcntl(pd[0], F_SETOWN, getpid()) < 0)
    {
        perror("fcntl F_SETOWN failed");
        exit(EXIT_FAILURE);
    }

    int flags = fcntl(pd[0], F_GETFL, 0);
    // Add Async and Non-blocking flags
    if (fcntl(pd[0], F_SETFL, flags | O_ASYNC | O_NONBLOCK) < 0)
    {
        perror("fcntl F_SETFL failed");
        exit(EXIT_FAILURE);
    }

    // 6. Register Signal Handler
    signal(SIGIO, piperead);

    len = sizeof(cliaddr);

    // 7. Main Loop
    while (1)
    {
        // Wait for command packet from client
        // The server blocks here until it gets a packet
        int n = recvfrom(sockfd, buffer, MAXLINE, 0, (struct sockaddr *)&cliaddr, &len);

        if (n > 0)
        {
            buffer[n] = '\0';
            // Execute the command
            // Output goes to stdout -> pipe -> triggers SIGIO -> piperead -> client
            system(buffer);

            // Allow a small delay for pipe to drain before next command (optional stability)
            usleep(10000);
        }
    }

    close(sockfd);
    return 0;
}