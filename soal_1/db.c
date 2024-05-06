#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <time.h>

#define SHM_SIZE 1024

void moveFiles() {
    // Buat shared memory
    key_t key = ftok("db.c", 65);
    int shmid = shmget(key, SHM_SIZE, IPC_CREAT | 0666);
    char *shmaddr = (char *)shmat(shmid, (void *)0, 0);

    // Baca daftar file yang lolos autentikasi dari shared memory
    char *filename = strtok(shmaddr, "\n");

    while (filename != NULL) {
        // Pindahkan file ke folder database
        char oldPath[100];
        char newPath[100];
        snprintf(oldPath, sizeof(oldPath), "new-data/%s", filename);
        snprintf(newPath, sizeof(newPath), "microservices/database/%s", filename);

        if (rename(oldPath, newPath) != 0) {
            perror("Error moving file");
            exit(EXIT_FAILURE);
        }

        // Log ke db.log
        FILE *logFile = fopen("microservices/database/db.log", "a");
        if (logFile == NULL) {
            perror("Error opening log file");
            exit(EXIT_FAILURE);
        }
        time_t now = time(NULL);
        struct tm *tm_info = localtime(&now);
        char buffer[26];
        strftime(buffer, 26, "%d/%m/%Y %H:%M:%S", tm_info);
        fprintf(logFile, "[%s] [%s] [%s]\n", buffer, strstr(filename, "trashcan") ? "Trash Can" : "Parking Lot", filename);
        fclose(logFile);

        filename = strtok(NULL, "\n");
    }

    // Hapus shared memory
    shmdt(shmaddr);
    shmctl(shmid, IPC_RMID, NULL);
}

int main() {
    moveFiles();
    return 0;
}
