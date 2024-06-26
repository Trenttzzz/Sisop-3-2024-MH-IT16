# Laporan Praktikum Modul 3

### Soal 1
1. Pertama kita akan membuat program `auth.c` yang akan memeriksa apakah file berakhiran "trashcan.csv" atau "parkinglot.csv". Jika file tidak memiliki format yang benar, program akan menghapus file tersebut dari direktori.
   Berikut adalah program dari `auth.c`:
   ```C
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
   ```

2. Kemudian kita lanjut dengan `db.c`, program ini memastikan bahwa hanya file-file dengan format yang benar yang diterima dan diproses.
   Berikut adalah program dari `db.c`:
   ```C
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
   ```

3. Terakhir, program `rate.c` program ini bertanggung jawab untuk menghitung Tempat Sampah dan Parkiran dengan rating terbaik dari file-file yang telah dipindahkan.
   Berikut adalah program dari `rate.c`:
   ```C
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
   ```

4. Setelah selesai mengerjakan ketiga program tersebut, kompilasi program tersebut menggunakan GCC:
   ```
   gcc -o auth auth.c -pthread
   gcc -o db db.c -pthread
   gcc -o rate rate.c -pthread
   ```


### Soal 2
1. Pertama saya akan memembuat program kalkulator yang dimana pada awalnya program saya simpan pada file bernama   
   `dudududu.c`.
2. Pada program `dudududu.c` akan terdiri dari *`child process`* dan *`parent process`* yang dimana pada *`parent process`* 
   akan mengubah input menjadi angka setelah pengoprasian kemudian pada *`child process`* berfungsi untuk mengubah hasil 
   angka menjadi kalimat. Dan berikut adalah program dari `dudududu.c`.
   ```C
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
   ```
3. Setelah program dijalankan menggunakan perintah
   ```bash
   gcc -o kalkulator dudududu.c
   ```
   maka selanjutnya program akan dijalankan dengan
   ```bash
   *./kalkulator -kali
   *./kalkulator -tambah
   *./kalkulator -kurang
   *./kalkulator -bagi
   ```
4. Contoh ketika program dijalankan adalah sebagai berikut :

   ![image](https://github.com/Trenttzzz/Sisop-3-2024-MH-IT16/assets/152785029/05200c91-2479-44f2-8eda-85b08f6a12c7)
   Maka semua program yang telah kita jalankan akan mucul semua hasilnya pada `histori.log`, dan berikut adalah isi dari 
   `histori.log` kita setelah menjalankan semua pengoprasian tadi

   ![image](https://github.com/Trenttzzz/Sisop-3-2024-MH-IT16/assets/152785029/2aaad53e-3dca-43b2-bac5-13f3bbf82d94)
6. Yang terakhir adalah kerika kita menjalankan hasil negatif maka program akan menampilkan pesan ERROR pada `histori.loh=g`
   nya. Berikut adalah contoh ketika kita menjalankan pengoprasian yang menghasilkan bilangan negatif

   ![image](https://github.com/Trenttzzz/Sisop-3-2024-MH-IT16/assets/152785029/cefd806a-2c94-481a-a6be-97f4cf35e75b)
   Maka akan menampilkan pesan pada `histori.log` sebagai berikut:

   ![image](https://github.com/Trenttzzz/Sisop-3-2024-MH-IT16/assets/152785029/e0572e66-5603-4cf8-ac2f-c5b148fc8e5c)
 
### Soal 3
#### Langkah - Langkah
1. Pada soal kita disuruh untuk membuat 3 file yaitu: `actions.c`, `paddock.c`, `driver.c`.
2. isi dari `actions.c` adalah fungsi-fungsi yang bakal di panggil oleh `paddock.c` yaitu:
   ```C
   #include <stdio.h>
   #include <string.h>

   // Function to handle Gap condition
   const char* Gap(float distance) {
      if (distance < 3.5)
          return "Gogogo";
      else if (distance >= 3.5 && distance < 10)
          return "Push";
      else
          return "Stay out of trouble";
    }

    // Function to handle Fuel condition
    const char* Fuel(int fuelPercentage) {
      if (fuelPercentage > 80)
          return "Push Push Push";
      else if (fuelPercentage >= 50 && fuelPercentage <= 80)
          return "You can go";
      else
          return "Conserve Fuel";
    }

    // Function to handle Tire condition
    const char* Tire(int tireUsage) {
      if (tireUsage > 80)
          return "Go Push Go Push";
      else if (tireUsage >= 50 && tireUsage <= 80)
          return "Good Tire Wear";
      else if (tireUsage >= 30 && tireUsage < 50)
          return "Conserve Your Tire";
      else
          return "Box Box Box";
    }

    // Function to handle Tire Change condition
    const char* TireChange(const char* tireType) {
      if (strcmp(tireType, "Soft") == 0)
          return "Mediums Ready";
      else if (strcmp(tireType, "Medium") == 0)
          return "Box for Softs";
      else
          return "Invalid tire type";
    }
    ```
   setiap fungsi diatas merepresentasikan logika yang diberi pada **soal nomor 3 opsi B**
3. lalu agar bisa membuat fungsi tersebut dipanggil oleh `paddock.c` kita harus membuat header nya yang akan di declare pada file `actions.h` yang berisi:
   ```C
   #ifndef ACTIONS_H
   #define ACTIONS_H

   // Function prototypes
   const char* Gap(float distance);
   const char* Fuel(int fuelPercentage);
   const char* Tire(int tireUsage);
   const char* TireChange(const char* tireType);

   #endif /* ACTIONS_H */

   ```
4. setelah itu kita bisa membuat file `driver.c` yang dia berisi fungsi untuk **menerima dan mengirim pesan** ke `paddock.c` berikut adalah kode nya:
   ```C
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

    // Example command: ./driver -c Gap -i 3.0
    if(argc != 5 || strcmp(argv[1], "-c") != 0 || strcmp(argv[3], "-i") != 0) {
        printf("Usage: %s -c [Command] -i [Value]\n", argv[0]);
        return -1;
    }

    // make argv[2](command) & argv[4](value) in one variable called command
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
    ```
    pada kode diatas kita define port 8080 untuk port nya, juga membuat socket dan juga connect ke paddock nya, lalu kita juga set command menggunakan argv dengan format `./driver -c <command> -i <value>`, dan diakhiri dengan menerima pesan yang dikirim dari paddock setelah mengirim command nya.
5. terakhir untuk file `paddock.c` nya ini berisi dengan logika pemrosesan command nya, serta membuat `logging message function` dan `daemonize function` berikut adalah kode nya:
    ```C
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
    
        char cwd[1024];
        if (getcwd(cwd, sizeof(cwd)) == NULL) {
            perror("Unable to get current working directory");
            exit(EXIT_FAILURE);
        }
    
        // Change working directory to current directory
        if (chdir(cwd) < 0)
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
    
        daemonize();
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
        
        char command[50]; // Deklarasi variabel di luar loop
        char value[50];
    
        // Creating socket file descriptor
        if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
            perror("socket failed");
            exit(EXIT_FAILURE);
        }
    
        // Forcefully attaching socket to the port 8080
        if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR , &opt, sizeof(opt))) {
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
    ```
    pada inti nya kurang lebih fungsinya sama seperti `driver.c` yang membedakan adalah disini juga memproses logika dari pemilihan command oleh driver lalu memanggil fungsi sesuai dengan command yang diminta.
   berikut adalah contoh penggunaan nya:
   ![image](https://github.com/Trenttzzz/Sisop-3-2024-MH-IT16/assets/141043792/23e704bd-2090-40a3-b380-652a8e7934f4)

   berikut juga adalah contoh dari log file nya:
   ```log
    [Driver] [11/05/2024 13:08:44]: [Received] [Gap 5.0]
    [Driver] [11/05/2024 13:08:44]: [Command] [Gap]
    [Paddock] [11/05/2024 13:08:44]: [Response] [Push]
   ```
### Soal 4
#### Langkah - Langkah
1. pada soal ini kita diberi sebuah file `csv` yang berisi:
   ```csv
   Rabu,Drama,Classroom of The Elite,ongoing
   Jumat,Adventure,Frieren,ongoing
   Senin,Action,Spy x Family,completed
   Minggu,Action,Solo Leveling,ongoing
   ```
   pada file tersebut disebutkan ada `<hari>, <genre>, <judul>, <status>`. nah pada soal ini kita juga disuruh untuk membuat 2 file yaitu `client.c` dan `server.c` yang mana file tersebut akan     mengimplementasikan CRUD pada list anime diatas.
2. pertama kita membuat `client.c` pada dasarnya file tersebut kurang lebih sama seperti fungsi pada nomer 3 yaitu dia bakal menerima input dan mengirimnya ke server, lalu menampilkan kembali apa yang dikirim dari server. berikut adalah isi dari file `client.c`
    ```C
    #include <stdio.h>
    #include <stdlib.h>
    #include <string.h>
    #include <unistd.h>
    #include <sys/socket.h>
    #include <arpa/inet.h>
    
    #define PORT 6000
    #define BUFFER_SIZE 8192
    
    int main() {
        int clientSocket;
        struct sockaddr_in serverAddr;
        char buffer[BUFFER_SIZE];
    
        // Membuat socket
        clientSocket = socket(AF_INET, SOCK_STREAM, 0);
        if (clientSocket < 0) {
            perror("socket");
            return 1;
        }
    
        // Mengonfigurasi alamat server
        memset(&serverAddr, 0, sizeof(serverAddr));
        serverAddr.sin_family = AF_INET;
        serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1");
        serverAddr.sin_port = htons(PORT);
    
        // Menghubungkan ke server
        if (connect(clientSocket, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) < 0) {
            perror("connect");
            return 1;
        }
    
        printf("Connected to server.\n");
    
        while (1) {
            printf("Enter command: ");
            fgets(buffer, BUFFER_SIZE, stdin);
            buffer[strcspn(buffer, "\n")] = '\0'; // Menghapus newline dari input
    
            // Mengirim perintah ke server
            send(clientSocket, buffer, strlen(buffer), 0);
    
            // Keluar dari program jika perintah "exit" diberikan
            if (strcmp(buffer, "exit") == 0) {
                break;
            }
    
            // Menerima dan mencetak pesan dari server
            memset(buffer, 0, BUFFER_SIZE);
            recv(clientSocket, buffer, BUFFER_SIZE, 0);
            printf("Received: \n"); // Menambahkan line break di sini
            puts(buffer); // print kiriman server
        }
    
        close(clientSocket);
        return 0;
    }
    ```
    pada kode diatas dia bakal menerima input command dari user dan mengirimnya ke server lalu menampilkan jawaban dari server menggunakan buffer.
3. pada `server.c` kita disuruh untuk membuat logika pemrosesan command yang diminta pada soal yaitu:
    - Menampilkan seluruh judul
    - Menampilkan judul berdasarkan genre
    - Menampilkan judul berdasarkan hari
    - Menampilkan status berdasarkan berdasarkan judul
    - Menambahkan anime ke dalam file myanimelist.csv
    - Melakukan edit anime berdasarkan judul
    - Melakukan delete berdasarkan judul
    - Selain command yang diberikan akan menampilkan tulisan “Invalid Command”
   berikut adalah implementasi command diatas pada kode nya:
    ```c
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
                    if (strstr(line, title) != NULL) {
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
    ```
    Fungsi pertama yaitu ada `LogAnimeChange` yang dimana fungsi tersebut bakal mencatat semua aktifitas user dan menulisnya pada log file. kemudian untuk fungsi lainnya adalah implementasi dari permintaan yang diminta pada soal.
4. kemudian saya juga menambahkan fungsi `haddle command` yang berfungsi untuk menjalankan logika dari pemilihan command user, jika user memilih **menampilkan semua anime** maka fungsi yang dijalankan adalah `displayAllAnime` begitu juga dengan yang lainnya, berikut adalah fungsinya:
    ```c
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
    ```
5. kemudian saya membuat fungsi `main` yang pada intinya dia membuat koneksi pada ip dan port tertentu kemudian menunggu koneksi dari client, jika sudah terkonek maka akan memberikan pesan ke client sesuai denga apa yang diminta oleh client, dan juga server ini tidak akan tertutup jika diberi kesalahan input. berikut adalah fungsinya:
    ```c
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
    ```
    dan juga berikut adalah contoh penggunaan, tampilan log file, serta penggunaan jika command nya salah
   ![image](https://github.com/Trenttzzz/Sisop-3-2024-MH-IT16/assets/141043792/14c222e7-ec01-43b1-8c75-5777613c9bb3)

    ```
    [06/05/24] [ADD] Senin,Fantasy,The Beginning After The End,ongoing
    [06/05/24] [ADD] Rabu,Adventure,Naruto,completed
    [06/05/24] [DEL] Naruto
    [06/05/24] [DEL] Attack on Titan
    ```

    ![image](https://github.com/Trenttzzz/Sisop-3-2024-MH-IT16/assets/141043792/a2f0e0a8-f36d-4821-9619-fa0b98efbf01)

