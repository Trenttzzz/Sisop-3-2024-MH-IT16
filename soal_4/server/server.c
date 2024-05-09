#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <time.h>

#define PORT 6000
#define BUFFER_SIZE 8192
#define CSV_FILE "../myanimelist.csv"
#define LOG_FILE "../change.log"
#define LINE_WITH_NUMBER_BUFFER_SIZE 10000

void logAnimeChange(const char *message, const char *type) {
    time_t now = time(NULL);
    struct tm *timeinfo = localtime(&now); // convert ke time local
    char timeStr[20];
    strftime(timeStr, sizeof(timeStr), "%d/%m/%y", timeinfo); // memasukkan time info sesuai template ke dalam timeStr

    FILE *logFile = fopen(LOG_FILE, "a");
    if (logFile != NULL) {
        fprintf(logFile, "[%s] [%s] %s\n", timeStr, type, message); // tulis time, type dan message kedalam logfile
        fclose(logFile);
    } else {
        perror("Error writing to log file");
    }
}

void displayAllAnime(int client_socket) {
    char buffer[BUFFER_SIZE];
    char command[BUFFER_SIZE];
    sprintf(command, "awk -F',' '{ print NR, $3 }' %s", CSV_FILE); // Memulai dari baris pertama 
    FILE *fp = popen(command, "r"); // membuat pointer ke FILE dan membuka pipe baru dimana output akan dikirim dari situ
    if (fp == NULL) {
        perror("Error executing command");
        strcpy(buffer, "Error executing command");
    } else {
        int line_number = 1;
        
        // loop untuk membaca semua baris yang ada pada fp
        while (fgets(buffer, BUFFER_SIZE - 16, fp) != NULL) {
            
            send(client_socket, buffer, strlen(buffer), 0); //  kirim buffer ke client
            
            line_number++;
        }
        pclose(fp);
    }
}

void displayAnimeByGenre(int client_socket, const char *genre) {
    char buffer[BUFFER_SIZE];
    char command[BUFFER_SIZE];
    sprintf(command, "awk -F',' '$2==\"%s\" { print $3 }' %s", genre, CSV_FILE); // Mencocokkan genre pada kolom kedua
    FILE *fp = popen(command, "r");
    if (fp == NULL) {
        perror("Error executing command");
        strcpy(buffer, "Error executing command");
    } else {
        int line_number = 1;
        while (fgets(buffer, BUFFER_SIZE, fp) != NULL) {
            
            // Menggabungkan nomor baris dengan isi baris
            char line_with_number[LINE_WITH_NUMBER_BUFFER_SIZE];
            snprintf(line_with_number, LINE_WITH_NUMBER_BUFFER_SIZE, "%d %s", line_number, buffer);
            // Mengirimkan respon ke client
            send(client_socket, line_with_number, strlen(line_with_number), 0);
            line_number++;
        }
        pclose(fp);
    }
}

void displayAnimeByDay(int client_socket, const char *day) {
    char buffer[BUFFER_SIZE];
    char command[BUFFER_SIZE];
    sprintf(command, "awk -F',' '$1==\"%s\" { print $3 }' %s", day, CSV_FILE); // Mencocokkan hari pada kolom pertama dan reformat command
    FILE *fp = popen(command, "r");
    if (fp == NULL) {
        perror("Error executing command");
        strcpy(buffer, "Error executing command");
    } else {
        int line_number = 1;
        
        while (fgets(buffer, BUFFER_SIZE, fp) != NULL) {

            // Menggabungkan nomor baris dengan isi baris
            char line_with_number[LINE_WITH_NUMBER_BUFFER_SIZE];
            snprintf(line_with_number, LINE_WITH_NUMBER_BUFFER_SIZE, "%d %s", line_number, buffer);
            // Mengirimkan respon ke client
            send(client_socket, line_with_number, strlen(line_with_number), 0);
            line_number++;

        }
        pclose(fp);
    }
}

void displayStatusByTitle(int client_socket, const char *title) {
    char buffer[BUFFER_SIZE];
    char command[BUFFER_SIZE * 2]; // Meningkatkan ukuran buffer

    // Membangun perintah awk dengan judul yang telah diberi delimiter
    snprintf(command, sizeof(command), "awk -F',' '$3~/%s/ { print $4 }' %s", title, CSV_FILE);

    FILE *fp = popen(command, "r");
    if (fp == NULL) {
        perror("Error executing command");
        strcpy(buffer, "Error executing command");
    } else {
        if (fgets(buffer, BUFFER_SIZE, fp) != NULL) {
            // Send response to client
            send(client_socket, buffer, strlen(buffer), 0);
        } else {
            // Jika tidak ada hasil yang dikembalikan, kirim pesan ke klien
            strcpy(buffer, "Anime not found");
            send(client_socket, buffer, strlen(buffer), 0);
        }
        pclose(fp);
    }
}

void addAnime(const char *animeDetails, int client_socket) {
    FILE *file = fopen(CSV_FILE, "a");
    if (file != NULL) {
        fprintf(file, "%s\n", animeDetails);
        fclose(file);
        // Kirim pesan ke client bahwa anime telah berhasil ditambahkan
        send(client_socket, "Anime berhasil ditambahkan\n", strlen("Anime berhasil ditambahkan\n"), 0);
    } else {
        perror("Error appending to file");
        send(client_socket, "Gagal menambahkan anime\n", strlen("Gagal menambahkan anime\n"), 0);
    }
}

void editAnime(const char *oldTitle, const char *newDetails, int client_socket) {
    FILE *file = fopen(CSV_FILE, "r");
    if (file != NULL) {
        FILE *tempFile = fopen("temp.csv", "w");
        if (tempFile != NULL) {
            char line[BUFFER_SIZE];
            int edited = 0; // Tandai apakah anime telah diedit
            while (fgets(line, BUFFER_SIZE, file) != NULL) {
                if (strstr(line, oldTitle) != NULL) {
                    
                    
                    fputs(newDetails, tempFile); // Tulis detail anime yang baru
                    edited = 1; // Tandai sebagai diedit
                    
                } 
                else {
                    fputs(line, tempFile); // Salin baris ke file sementara tanpa mengubahnya
                }
            }
            fclose(tempFile);
            fclose(file);
            remove(CSV_FILE);
            rename("temp.csv", CSV_FILE);
            char buffer[BUFFER_SIZE];
            if (edited) {
                strcpy(buffer, "Anime berhasil diubah\n");
            } else {
                strcpy(buffer, "Anime tidak ditemukan\n");
            }
            send(client_socket, buffer, strlen(buffer), 0); // Kirim pesan kembali ke klien
        } else {
            perror("Error creating temp file");
        }
    } else {
        perror("Error opening file for editing");
    }
}


void deleteAnime(const char *title, int client_socket) {
    FILE *file = fopen(CSV_FILE, "r");
    if (file != NULL) {
        FILE *tempFile = fopen("temp.csv", "w");
        if (tempFile != NULL) {
            char line[BUFFER_SIZE];
            int deleted = 0; // Tambahkan variabel deleted untuk menandai apakah anime telah dihapus
            while (fgets(line, BUFFER_SIZE, file) != NULL) {
                // Menghapus karakter newline di akhir baris
                char *newline = strchr(line, '\n');
                if (newline != NULL) {
                    *newline = '\0';
                }

                // Membandingkan judul secara keseluruhan dengan judul yang diberikan
                if (strcmp(line, title) != 0) {
                    fputs(line, tempFile);
                } else {
                    deleted = 1; // Jika judul ditemukan, tandai sebagai dihapus
                }
            }
            fclose(tempFile);
            fclose(file);
            remove(CSV_FILE);
            rename("temp.csv", CSV_FILE);
            char buffer[BUFFER_SIZE];
            if (deleted) {
                strcpy(buffer, "Anime berhasil dihapus\n");
            } else {
                strcpy(buffer, "Anime tidak ditemukan\n");
            }
            send(client_socket, buffer, strlen(buffer), 0); // Kirim pesan kembali ke klien
        } else {
            perror("Error creating temp file");
        }
    } else {
        perror("Error opening file for deletion");
    }
}


void handle_command(char *command, int client_socket) {
    char buffer[BUFFER_SIZE] = {0};

    // Proses perintah
    if (strcmp(command, "tampilkan") == 0) {
        displayAllAnime(client_socket);
    } else if (strncmp(command, "genre ", 6) == 0) {
        char *genre = strtok(command + 6, " ");
        displayAnimeByGenre(client_socket, genre);
    } else if (strncmp(command, "hari ", 5) == 0) {
        char *day = strtok(command + 5, " ");
        displayAnimeByDay(client_socket, day);
    } else if (strncmp(command, "status ", 7) == 0) {
        char *title = strtok(command + 7, " ");
        displayStatusByTitle(client_socket, title);
    } else if (strncmp(command, "add ", 4) == 0) {
        addAnime(command + 4, client_socket);
        logAnimeChange(command + 4, "ADD");
    } else if (strncmp(command, "edit ", 5) == 0) {
        char *oldDetails = strtok(command + 5, ",");
        char *newDetails = strtok(NULL, "");
        editAnime(oldDetails, newDetails, client_socket);
        logAnimeChange(newDetails, "EDIT");
    } else if (strncmp(command, "delete ", 7) == 0) {
        deleteAnime(command + 7, client_socket);
        logAnimeChange(command + 7, "DEL");
    } else {
        strcpy(buffer, "Invalid Command");
        send(client_socket, buffer, strlen(buffer), 0);
    }
}

int main() {
    int server_fd, new_socket;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);
    char buffer[BUFFER_SIZE] = {0};

    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR , &opt, sizeof(opt))) {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }
    if (listen(server_fd, 3) < 0) {
        perror("listen");
        exit(EXIT_FAILURE);
    }
    if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen)) < 0) {
        perror("accept");
        exit(EXIT_FAILURE);
    }

    printf("Connected to client\n");

    while (1) {
        memset(buffer, 0, BUFFER_SIZE);
        if (read(new_socket, buffer, BUFFER_SIZE) <= 0) {
            perror("read");
            break;
        }
        printf("Received command: %s\n", buffer);
        handle_command(buffer, new_socket);
        if (strcmp(buffer, "exit\n") == 0) {
            printf("Exiting...\n");
            break;
        }
        printf("Enter command: ");
        
        fflush(stdout); // Flush output buffer agar prompt muncul di layar
    }

    close(new_socket);
    close(server_fd);
    return 0;
}
