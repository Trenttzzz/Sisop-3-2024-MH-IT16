#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <unistd.h>

void authenticateFiles() {
    DIR *dir;
    struct dirent *entry;

    // Buka direktori new-data
    dir = opendir("new-data");
    if (dir == NULL) {
        perror("Error opening directory");
        exit(EXIT_FAILURE);
    }

    // Loop melalui setiap file dalam direktori
    while ((entry = readdir(dir)) != NULL) {
        if (entry->d_type == DT_REG) { // Hanya berlaku untuk file regular
            char *filename = entry->d_name;
            int len = strlen(filename);

            // Pastikan nama file berakhiran "trashcan.csv" atau "parkinglot.csv"
            if ((len > 10 && strcmp(filename + len - 10, "trashcan.csv") == 0) ||
                (len > 12 && strcmp(filename + len - 12, "parkinglot.csv") == 0)) {
                // File lolos autentikasi, tidak perlu melakukan apa-apa
            } else {
                // Hapus file yang tidak sesuai dengan format
                char path[100];
                snprintf(path, sizeof(path), "new-data/%s", filename);
                remove(path);
            }
        }
    }

    closedir(dir);
}

int main() {
    authenticateFiles();
    return 0;
}
