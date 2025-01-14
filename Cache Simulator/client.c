#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdbool.h>

// stats function
int stats(const char *op, int nums[5])
{
    // simple max check
    if (strcmp(op, "max") == 0)
    {
        int max = nums[0];
        for (int i = 1; i < 5; i++)
        {
            if (nums[i] > max)
                max = nums[i];
        }
        return max;
    }
    // simple min check
    else if (strcmp(op, "min") == 0)
    {
        int min = nums[0];
        for (int i = 1; i < 5; i++)
        {
            if (nums[i] < min)
                min = nums[i];
        }
        return min;
    }
    // sorts for median
    else if (strcmp(op, "median") == 0)
    {
        for (int i = 0; i < 4; i++)
        {
            for (int j = i + 1; j < 5; j++)
            {
                if (nums[i] > nums[j])
                {
                    int temp = nums[i];
                    nums[i] = nums[j];
                    nums[j] = temp;
                }
            }
        }
        return nums[2];
    }
}
// main function
int main(int argc, char *argv[])
{
    // check for correct number of arguments
    if (argc != 4)
    {
        fprintf(stderr, "Usage: %s <email> <port> <host_ip>\n", argv[0]);
        return EXIT_FAILURE;
    }

    // assign each
    char *ip = argv[3];
    char *email = argv[1];
    int port = atoi(argv[2]);

    // create socket
    int sock = socket(AF_INET, SOCK_STREAM, 0);

    // server address
    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    inet_pton(AF_INET, ip, &server_addr.sin_addr);

    // connect
    connect(sock, (struct sockaddr *)&server_addr, sizeof(server_addr));

    // send message
    char id_message[256];
    snprintf(id_message, sizeof(id_message), "cs230 HELLO %s\n", email);
    send(sock, id_message, strlen(id_message), 0);

    // process stats problems
    char temp[256];
    while(true)
    {
        memset(temp, 0, sizeof(temp));
        ssize_t received = recv(sock, temp, sizeof(temp) - 1, 0);

        if (strncmp(temp, "cs230", 5) == 0 && strstr(temp, "BYE"))
        {
            printf("Flag received: %s", temp);
            break;
        }
        char op[16];
        int nums[5];
        sscanf(temp, "cs230 STATUS %s %d %d %d %d %d", op, &nums[0], &nums[1], &nums[2], &nums[3], &nums[4]);
        int final = stats(op, nums);
        char sendback[256];
        snprintf(sendback, sizeof(sendback), "cs230 %d\n", final);
        send(sock, sendback, strlen(sendback), 0);
    }
    close(sock);
}
