#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <time.h> // Tambahkan untuk mendapatkan waktu saat ini
#include <sys/stat.h> // Tambahkan untuk membuat direktori jika belum ada
#include "actions.h" // Include actions.h header file
#include <sys/types.h>
#include <sys/stat.h>

#define PORT 8080
#define LOG_FILE "race.log" // Ubah path file log

void daemonize() {
    pid_t pid;

    // Fork off the parent process
    pid = fork();

    // An error occurred
    if (pid < 0)
        exit(EXIT_FAILURE);

    // Success: Let the parent terminate
    if (pid > 0)
        exit(EXIT_SUCCESS);

    // On child process
    // Create new session and process group
    if (setsid() < 0)
        exit(EXIT_FAILURE);

    // Set the working directory to root
    if (chdir("/") < 0)
        exit(EXIT_FAILURE);

    // Close all open file descriptors
    close(STDIN_FILENO);
    close(STDOUT_FILENO);
    close(STDERR_FILENO);
}

// Fungsi untuk mencatat log dengan format yang diminta
void log_message(const char *source, const char *command, const char *additional_info) {
    FILE *log_file = fopen(LOG_FILE, "a"); // Buka file log untuk ditambahkan
    if (log_file == NULL) {
        perror("Unable to open log file");
        return;
    }

    time_t now;
    struct tm *tm_info;
    char buffer[26];
    time(&now);
    tm_info = localtime(&now);
    strftime(buffer, 26, "%d/%m/%Y %H:%M:%S", tm_info);

    // Cetak pesan log ke file
    fprintf(log_file, "[%s] [%s]: [%s] [%s]\n", source, buffer, command, additional_info);

    fclose(log_file); // Tutup file log
}

int main() {

    //daemonize();
    // Membuat direktori jika belum ada
    struct stat st = {0};
    /*if (stat("server/logs", &st) == -1) {
        mkdir("server/logs", 0700);
    }*/

    int server_fd, new_socket, valread;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);
    char buffer[1024] = {0};
    char *hello = "Hello from server";
    char command[50]; // Deklarasi variabel di luar loop
    char value[50];

    // Creating socket file descriptor
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    // Forcefully attaching socket to the port 8080
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT,
                                                  &opt, sizeof(opt))) {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons( PORT );

    // Forcefully attaching socket to the port 8080
    if (bind(server_fd, (struct sockaddr *)&address,
                                 sizeof(address))<0) {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }
    if (listen(server_fd, 3) < 0) {
        perror("listen");
        exit(EXIT_FAILURE);
    }

    // Daemon process
    printf("Listening to %d...\n", PORT);

    while(1) {
        // Accept connection from driver.c
        if ((new_socket = accept(server_fd, (struct sockaddr *)&address,
                           (socklen_t*)&addrlen))<0) {
            perror("accept");
            exit(EXIT_FAILURE);
        }

        // Read command from driver.c
        valread = read(new_socket, buffer, 1024);
        log_message("Driver", "Received", buffer); // Catat log untuk pesan yang diterima

        // Parse command and value
        sscanf(buffer, "%s %s", command, value);
        log_message("Driver", "Command", command); // Catat log untuk perintah yang diterima

        // Process command
        if (strcmp(command, "Gap") == 0) {
            const char *response = Gap((float)atof(value));
            char paddock_response[1024];
            sprintf(paddock_response, "[Paddock]: %s", response);
            send(new_socket, paddock_response, strlen(paddock_response), 0);
            log_message("Paddock", "Response", response); // Catat log untuk respon yang dikirim
        } else if (strcmp(command, "Fuel") == 0) {
            const char *response = Fuel(atoi(value));
            char paddock_response[1024];
            sprintf(paddock_response, "[Paddock]: %s", response);
            send(new_socket, paddock_response, strlen(paddock_response), 0);
            log_message("Paddock", "Response", response); // Catat log untuk respon yang dikirim
        } else if (strcmp(command, "Tire") == 0) {
            const char *response = Tire(atoi(value));
            char paddock_response[1024];
            sprintf(paddock_response, "[Paddock]: %s", response);
            send(new_socket, paddock_response, strlen(paddock_response), 0);
            log_message("Paddock", "Response", response); // Catat log untuk respon yang dikirim
        } else if (strcmp(command, "TireChange") == 0) {
            const char *response = TireChange(value);
            char paddock_response[1024];
            sprintf(paddock_response, "[Paddock]: %s", response);
            send(new_socket, paddock_response, strlen(paddock_response), 0);
            log_message("Paddock", "Response", response); // Catat log untuk respon yang dikirim
        } else {
            char paddock_response[1024] = "[Paddock]: Invalid command";
            send(new_socket, paddock_response, strlen(paddock_response), 0);
            log_message("Paddock", "Response", "Invalid command"); // Catat log untuk respon yang dikirim
        }

        // Close socket after processing the command
        close(new_socket);
    }

    return 0;
}
