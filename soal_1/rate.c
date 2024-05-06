#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <string.h>

#define SHM_SIZE 1024

void rateBest() {
    // Buat shared memory
    key_t key = ftok("db.c", 65);
    int shmid = shmget(key, SHM_SIZE, 0666);
    char *shmaddr = (char *)shmat(shmid, (void *)0, 0);

    // Lakukan perhitungan rating terbaik di sini
    printf("Menghitung Tempat Sampah dan Parkiran dengan Rating Terbaik...\n");

    // Simpan hasil rating terbaik di sini

    // Tulis hasil rating terbaik ke stdout
    printf("Tempat Sampah dan Parkiran dengan Rating Terbaik:\n");
    printf("Trash Can: ...\n");
    printf("Parking Lot: ...\n");

    // Hapus shared memory
    shmdt(shmaddr);
}

int main() {
    rateBest();
    return 0;
}
