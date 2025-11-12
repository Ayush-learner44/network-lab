#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>

struct entry
{
    int key;
    const char *name;
} db[] = {
    {0, "Bob"}, {3, "Anne"}, {5, "Barb"}, {7, "Ray"}, {9, "Denbigh"}, {10, "Terri"}, {104, "John"}};
int dbsize = sizeof(db) / sizeof(db[0]);

const char *lookup_by_address(int key)
{
    for (int i = 0; i < dbsize; i++)
        if (db[i].key == key)
            return db[i].name;
    return NULL;
}

int lookup_by_name(const char *name)
{
    for (int i = 0; i < dbsize; i++)
        if (strcasecmp(db[i].name, name) == 0)
            return db[i].key;
    return -1;
}

int main()
{
    int s, c;
    struct sockaddr_in addr;
    char buf[200];

    s = socket(AF_INET, SOCK_STREAM, 0);

    addr.sin_family = AF_INET;
    addr.sin_port = htons(8080);
    addr.sin_addr.s_addr = INADDR_ANY;

    bind(s, (struct sockaddr *)&addr, sizeof(addr));
    listen(s, 5);

    printf("Server ready on port 8080...\n");

    while (1)
    {
        c = accept(s, NULL, NULL);
        while (1)
        {
            int n = read(c, buf, sizeof(buf) - 1);
            if (n <= 0)
                break;

            unsigned char code = buf[0];
            if (buf[n - 1] != 0x03)
            {
                write(c, "Bad request\003", 12);
                continue;
            }
            buf[n - 1] = 0; // remove ETX
            char *data = &buf[1];

            if (code == 0x09)
            { // number lookup
                int key = atoi(data);
                const char *ans = lookup_by_address(key);
                if (ans)
                {
                    write(c, ans, strlen(ans));
                    write(c, "\003", 1);
                }
                else
                {
                    write(c, "Address not found\003", 20);
                }
            }
            else if (code == 0x01)
            { // name lookup
                int key = lookup_by_name(data);
                if (key >= 0)
                {
                    char num[20];
                    int len = 0;
                    // convert int to string manually with printf
                    len = sprintf(num, "%d", key); // okay, if you hate sprintf, you can printf to stdout, but we need string for socket
                    write(c, num, len);
                    write(c, "\003", 1);
                }
                else
                {
                    write(c, "Name not found\003", 15);
                }
            }
            else
            {
                write(c, "Unknown code\003", 13);
            }
        }
        close(c);
    }
}
