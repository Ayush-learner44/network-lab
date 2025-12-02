The errors you are seeing (`[cite_start]`, `expected expression`) happened because some text formatting tags from the explanation accidentally got copied into your code file. These are not valid C code.

Here is the **cleaned Server code** (removing all those errors) and the **Client code** written in the style you asked for, but adapted for **UDP** and **Question 2**.

### 1\. The Clean Server Code (`server.c`)

This is the exact logic needed for Assignment 6 Question 2. It captures the `system()` output using a pipe and signals, then sends it back to the client.

http://googleusercontent.com/immersive_entry_chip/0

### 2\. The Client Code (`client.c`)

I have adapted your client code style to match the **Assignment 6 Question 2** requirements:

1.  **Arguments:** It takes `<host>` and `<command>` as arguments (required by PDF).
2.  **UDP:** Uses `SOCK_DGRAM` instead of `SOCK_STREAM`.
3.  **`gethostbyname`**: Uses this instead of `inet_pton` (required by PDF Question 1 & 2).

http://googleusercontent.com/immersive_entry_chip/1

### How to Run

1.  **Compile Server:**
    ````bash
    gcc server.c -o server
    2.  **Compile Client:**
    ```bash
    gcc client.c -o execute
    3.  **Run Server:**
    ```bash
    ./server
    4.  **Run Client (in a new terminal):**
    ```bash
    ./execute localhost "ls"
        *(You can replace "ls" with "pwd" or "date" to test)*
    ````