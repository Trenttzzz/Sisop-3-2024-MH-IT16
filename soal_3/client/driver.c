#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 8080

int main(int argc, char *argv[]) {
    struct sockaddr_in address;
    int sock = 0, valread;
    struct sockaddr_in serv_addr;
    char buffer[1024] = {0};

    // Create socket
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        printf("\n Socket creation error \n");
        return -1;
    }

    memset(&serv_addr, '0', sizeof(serv_addr));

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);

    // Convert IPv4 and IPv6 addresses from text to binary form
    if(inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0) {
        printf("\nInvalid address/ Address not supported \n");
        return -1;
    }

    // Connect to server
    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        printf("\nConnection Failed \n");
        return -1;
    }

    // Example command: ./driver -c Gap -f 3.0
    if(argc != 5 || strcmp(argv[1], "-c") != 0) {
        printf("Usage: %s -c [Command] -f [Value]\n", argv[0]);
        return -1;
    }

    char command[1024];
    sprintf(command, "%s %s", argv[2], argv[4]);

    // Send command to paddock.c
    send(sock, command, strlen(command), 0);
    printf("[Driver]: %s\n", command);

    // Receive response from paddock.c
    valread = read(sock, buffer, 1024);
    printf("%s\n", buffer);

    return 0;
}
