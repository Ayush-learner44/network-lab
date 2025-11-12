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
    char buf[100];

    s = socket(AF_INET, SOCK_STREAM, 0);

    addr.sin_family = AF_INET;
    addr.sin_port = htons(8080);       // same as client
    addr.sin_addr.s_addr = INADDR_ANY; // accept from any IP

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
            buf[n] = 0;
            buf[strcspn(buf, "\n")] = 0;

            int isnum = 1;
            for (int i = 0; buf[i]; i++)
                if (!isdigit((unsigned char)buf[i]))
                {
                    isnum = 0;
                    break;
                }

            if (isnum)
            {
                int key = atoi(buf);
                const char *ans = lookup_by_address(key);
                if (ans)
                    write(c, ans, strlen(ans));
                else
                    write(c, "Address not found", 17);
            }
            else
            {
                int key = lookup_by_name(buf);
                if (key >= 0)
                {
                    char out[20];
                    int len = snprintf(out, sizeof(out), "%d", key);
                    write(c, out, len);
                }
                else
                {
                    write(c, "Name not found", 14);
                }
            }
        }
        close(c);
    }
}
