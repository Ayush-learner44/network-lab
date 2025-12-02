Here is the implementation of **Assignment \#6, Question 2**.

[cite_start]This code merges the **UDP Server** requirements [cite: 20, 22] [cite_start]with the **Asynchronous Pipe/Signal** logic provided in the PDF [cite: 43-125].

### Key Concepts Used

1.  **`dup(pd[1])`**: We trick the OS into thinking the "pipe" is the screen. [cite_start]When `system()` prints output, it goes into the pipe [cite: 60-61].
2.  [cite_start]**`signal(SIGIO, ...)`**: Since `system()` blocks the program, we use a signal to interrupt us whenever data appears in the pipe[cite: 108, 145].
3.  **Global Variables**: The signal handler (`piperead`) needs to send data back to the client, but it can't accept arguments. We must store the client's address in global variables.

### The Code (`udp_server.c`)

```c
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

#define MAXLINE 1024
#define PORT 8080

// Global variables (Required because signal handlers can't take custom arguments)
int sockfd;
struct sockaddr_in cliaddr;
socklen_t len;
int pd[2]; [cite_start]// The pipe descriptors [cite: 45]

// --- Signal Handler Function ---
// This function triggers automatically when the command writes data to the pipe.
// It reads from the pipe and sends the data immediately to the client over UDP.
void piperead(int sig) {
    char buf[MAXLINE];
    int n;

    [cite_start]// Read from the read-end of the pipe (pd[0]) [cite: 141]
    // We use a loop to ensure we drain the pipe if multiple chunks are ready
    while ((n = read(pd[0], buf, MAXLINE - 1)) > 0) {
        buf[n] = '\0';
        // Instead of fprintf to stderr (as in PDF), we send back to client
        sendto(sockfd, buf, n, 0, (const struct sockaddr *)&cliaddr, len);
    }
}

int main() {
    struct sockaddr_in servaddr;
    char buffer[MAXLINE];
    int saved_stdout; // To restore the real terminal output later if needed

    [cite_start]// 1. Create UDP Socket (DGRAM) [cite: 20]
    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    memset(&servaddr, 0, sizeof(servaddr));
    memset(&cliaddr, 0, sizeof(cliaddr));

    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = INADDR_ANY;
    servaddr.sin_port = htons(PORT);

    // 2. Bind the socket
    if (bind(sockfd, (const struct sockaddr *)&servaddr, sizeof(servaddr)) < 0) {
        perror("Bind failed");
        exit(EXIT_FAILURE);
    }

    printf("UDP Server listening on port %d...\n", PORT);

    [cite_start]// 3. Setup the Pipe [cite: 44]
    if (pipe(pd) < 0) {
        perror("Pipe failed");
        exit(EXIT_FAILURE);
    }

    // 4. "The Dup Trick" - Redirect stdout to the pipe
    // First, save the actual stdout so we can restore it if we want to print server logs
    saved_stdout = dup(STDOUT_FILENO); 

    [cite_start]// Close real stdout (fd 1) [cite: 59]
    close(STDOUT_FILENO);

    [cite_start]// Duplicate the write-end of pipe (pd[1]) to fd 1 [cite: 60]
    // Now, any printf or system() output goes to pd[1]
    if (dup(pd[1]) != STDOUT_FILENO) {
        perror("Dup failed"); // Should print to stderr since stdout is closed
        exit(EXIT_FAILURE);
    }
    
    // We don't need the original pd[1] reference anymore, it's now accessible via fd 1
    // However, keep it open so the pipe remains valid.

    [cite_start]// 5. Setup Asynchronous I/O on the Read-End (pd[0]) [cite: 105-106]
    // This ensures the program gets a signal when data is available to read
    if (fcntl(pd[0], F_SETOWN, getpid()) < 0) {
        perror("fcntl F_SETOWN failed");
        exit(EXIT_FAILURE);
    }
    int flags = fcntl(pd[0], F_GETFL, 0);
    fcntl(pd[0], F_SETFL, flags | O_ASYNC | O_NONBLOCK); 
    // Note: O_NONBLOCK prevents the handler from hanging if the pipe is momentarily empty

    [cite_start]// 6. Register the Signal Handler [cite: 112]
    signal(SIGIO, piperead);

    len = sizeof(cliaddr);

    // 7. Main Server Loop
    while (1) {
        // Receive command from client
        // Note: This recvfrom might be interrupted by the signal, so we handle that.
        int n = recvfrom(sockfd, buffer, MAXLINE, 0, (struct sockaddr *)&cliaddr, &len);
        
        if (n > 0) {
            buffer[n] = '\0';
            
            [cite_start]// Execute the command [cite: 29]
            // Output will go to stdout -> pipe -> trigger SIGIO -> piperead -> sendto client
            system(buffer);
            
            // Send a "done" marker or just let the client timeout. 
            // For this lab, simply executing is usually enough.
            const char *msg = "\n[Command Execution Finished]\n";
            sendto(sockfd, msg, strlen(msg), 0, (const struct sockaddr *)&cliaddr, len);
        }
    }

    close(sockfd);
    return 0;
}
```

### How to Run This

**1. Compile the Server:**

```bash
gcc udp_server.c -o server
```

**2. Run the Server:**

```bash
./server
```

*(Note: You won't see much output on the server screen because we redirected `stdout` to the network\!)*

**3. Run the Client:**
You can write a simple python client or use `netcat` (nc) to test it quickly:

```bash
nc -u 127.0.0.1 8080
```

Type `ls` or `pwd` and hit enter. You should see the directory listing come back to you.

### Important Implementation Note for your Lab

[cite_start]The PDF code uses `fcntl(..., FASYNC | O_NDELAY)`[cite: 106].

  * In modern Linux, `O_NDELAY` is essentially `O_NONBLOCK`.
  * I added `O_NONBLOCK` in the code above. This is crucial because if the signal handler tries to `read()` when the pipe is empty, it might block the whole process, freezing your server. The non-blocking flag prevents this.

Would you like the Python client code to test this as well, or are you comfortable using `netcat`?