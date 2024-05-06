#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>

#define BUFFER_SIZE 256

// Konversi nama angka ke bilangan bulat
int convertStringToNumber(const char *str) {
    if (strcmp(str, "satu") == 0) {
        return 1;
    } else if (strcmp(str, "dua") == 0) {
        return 2;
    } else if (strcmp(str, "tiga") == 0) {
        return 3;
    } else if (strcmp(str, "empat") == 0) {
        return 4;
    } else if (strcmp(str, "lima") == 0) {
        return 5;
    } else if (strcmp(str, "enam") == 0) {
        return 6;
    } else if (strcmp(str, "tujuh") == 0) {
        return 7;
    } else if (strcmp(str, "delapan") == 0) {
        return 8;
    } else if (strcmp(str, "sembilan") == 0) {
        return 9;
    } else {
        return -1;
    }
}

// Konversi bilangan bulat ke kalimat bahasa Indonesia
void convertNumberToWords(int num, char *buffer) {
    const char *angka[] = {
        "nol", "satu", "dua", "tiga", "empat", "lima", "enam", "tujuh", "delapan", "sembilan",
        "sepuluh", "sebelas", "dua belas", "tiga belas", "empat belas", "lima belas",
        "enam belas", "tujuh belas", "delapan belas", "sembilan belas"
    };

    if (num < 20) {
        strcpy(buffer, angka[num]);
    } else {
        const char *puluhan[] = {
            "", "", "dua puluh", "tiga puluh", "empat puluh", "lima puluh",
            "enam puluh", "tujuh puluh", "delapan puluh", "sembilan puluh"
        };
        int tens = num / 10;
        int units = num % 10;
        strcpy(buffer, puluhan[tens]);
        if (units != 0) {
            strcat(buffer, " ");
            strcat(buffer, angka[units]);
        }
    }
}

// Fungsi logic kalkulator
int performOperation(const char *operation, int num1, int num2, int *result) {
    if (strcmp(operation, "-kali") == 0) {
        *result = num1 * num2;
        return 1;
    } else if (strcmp(operation, "-tambah") == 0) {
        *result = num1 + num2;
        return 1;
    } else if (strcmp(operation, "-kurang") == 0) {
        *result = num1 - num2;
        if (*result < 0) {
            return 0; // error jika negatif
        }
        return 1;
    } else if (strcmp(operation, "-bagi") == 0) {
        if (num2 == 0) {
            return 0; // error jika pembagi nol
        }
        *result = num1 / num2; // hasil pembagian harus bulat
        return 1;
    } else {
        return 0; 
    }
}

// Fungsi waktu & tanggal saat ini
void getCurrentDateTime(char *dateTime) {
    time_t now = time(NULL);
    struct tm *tmInfo = localtime(&now);
    strftime(dateTime, BUFFER_SIZE, "%d/%m/%y %H:%M:%S", tmInfo);
}

// kalimat hasil kalkulator
void formatSentence(char *sentence, const char *operation, const char *num1Str, const char *num2Str, const char *resultPhrase) {
    if (strcmp(operation, "-kali") == 0) {
        snprintf(sentence, BUFFER_SIZE, "hasil perkalian %s dan %s adalah %s.", num1Str, num2Str, resultPhrase);
    } else if (strcmp(operation, "-tambah") == 0) {
        snprintf(sentence, BUFFER_SIZE, "hasil penjumlahan %s dan %s adalah %s.", num1Str, num2Str, resultPhrase);
    } else if (strcmp(operation, "-kurang") == 0) {
        snprintf(sentence, BUFFER_SIZE, "hasil pengurangan %s dan %s adalah %s.", num2Str, num1Str, resultPhrase);
    } else if (strcmp(operation, "-bagi") == 0) {
        snprintf(sentence, BUFFER_SIZE, "hasil pembagian %s dan %s adalah %s.", num1Str, num2Str, resultPhrase);
    }
}

int main(int argc, char *argv[]) {
    if (argc != 4) {
        printf("Usage: ./dudududu -operation num1 num2\n");
        printf("Operations: -kali, -tambah, -kurang, -bagi\n");
        return EXIT_FAILURE;
    }

    const char *operation = argv[1];
    const char *num1Str = argv[2];
    const char *num2Str = argv[3];

    // string jadi angka
    int num1 = convertStringToNumber(num1Str);
    int num2 = convertStringToNumber(num2Str);

    // memeriksa string valid atau tidak
    if (num1 == -1 || num2 == -1) {
        printf("Invalid input: Please enter valid numbers (satu sampai sembilan).\n");
        return EXIT_FAILURE;
    }

    int parentToChildPipe[2];
    int childToParentPipe[2];
    if (pipe(parentToChildPipe) == -1 || pipe(childToParentPipe) == -1) {
        perror("Failed to create pipes");
        return EXIT_FAILURE;
    }

    pid_t pid = fork();

    if (pid < 0) {
        perror("Failed to fork process");
        return EXIT_FAILURE;
    }

    if (pid == 0) {
        // Child process
        close(parentToChildPipe[1]); 
        close(childToParentPipe[0]); 

        int result;
        read(parentToChildPipe[0], &result, sizeof(result)); 

        char resultPhrase[BUFFER_SIZE];
        convertNumberToWords(result, resultPhrase);

        char sentence[BUFFER_SIZE];
        formatSentence(sentence, operation, num1Str, num2Str, resultPhrase);

        write(childToParentPipe[1], sentence, strlen(sentence) + 1);

        close(parentToChildPipe[0]);
        close(childToParentPipe[1]);

        return EXIT_SUCCESS;
    } else {
        // Parent process
        close(parentToChildPipe[0]); 
        close(childToParentPipe[1]); 

        int result;
        if (!performOperation(operation, num1, num2, &result)) {
            // menulis error apabila terjadi error yang dicatatakan pada log
            char dateTime[BUFFER_SIZE];
            getCurrentDateTime(dateTime);
            FILE *logFile = fopen("histori.log", "a");
            if (logFile != NULL) {
                fprintf(logFile, "[%s] [%s] ERROR\n", dateTime, operation);
                fclose(logFile);
            }

            close(parentToChildPipe[1]);
            close(childToParentPipe[0]);

            wait(NULL); 
            return EXIT_SUCCESS;
        }

        write(parentToChildPipe[1], &result, sizeof(result));

        char sentence[BUFFER_SIZE];
        read(childToParentPipe[0], sentence, BUFFER_SIZE);

        // waktu & tanggal saat ini
        char dateTime[BUFFER_SIZE];
        getCurrentDateTime(dateTime);

        // menulis kalimat ke log
        FILE *logFile = fopen("histori.log", "a");
        if (logFile != NULL) {
            fprintf(logFile, "[%s] [%s] %s\n", dateTime, operation, sentence);
            fclose(logFile);
        }

        close(parentToChildPipe[1]);
        close(childToParentPipe[0]);

        wait(NULL);
    }

    return EXIT_SUCCESS;
}
