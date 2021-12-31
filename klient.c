//
// Created by Matúš Baláži on 28. 12. 2021.
//

#include "klient.h"
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define BUFFER_SIZE 256
#define NAME_LENGTH 30

// volatile sig_atomic_t flag = 0;
int sockfd = 0;
char name[NAME_LENGTH];


void recv_msg_handler() { //

    char message[BUFFER_SIZE] = {};

    while (1) {
        int receive = read(sockfd, message, BUFFER_SIZE);
        //int receive = recv(sockfd, message, BUFFER_SIZE, 0);
        if (receive > 0) {
            printf("%s \n", message);
        } else if (receive == 0) {
            break;
        } else {
            printf("SOMETHING BAD HAPPENED :(\n");
        }
        bzero(message, BUFFER_SIZE);
    }
}


void send_msg_handler() {
    char buffer[BUFFER_SIZE] = {};
    //char message[BUFFER_SIZE + NAME_LENGTH] = {};

    /*while (1) {
        str_overwrite_stdout();
        fgets(buffer, BUFFER_SIZE, stdin);
        str_trim_lf(buffer, BUFFER_SIZE);

        if (strcmp(buffer, "exit") == 0) {
            break;
        } else {
            sprintf(message, "%s: %s\n", name, buffer);
            send(sockfd, message, strlen(message), 0);
        }
        bzero(buffer, BUFFER_SIZE);
        bzero(message, BUFFER_SIZE + NAME_LENGTH);
    }*/

    chatovanie(buffer, sockfd);
}


int main(int argc, char *argv[]) {
    setlinebuf(stdout);
    int sockfd, n;
    struct sockaddr_in serv_addr;
    struct hostent *server;
    char buffer[256];

    pthread_t send_msg_thread, recv_msg_thread;

    if (argc < 3) {
        fprintf(stderr, "usage %s hostname port\n", argv[0]);
        return 1;
    }

    server = gethostbyname(argv[1]);
    if (server == NULL) {
        fprintf(stderr, "Error, no such host\n");
        return 2;
    }

    // SOCKET SETTINGS
    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy(
            (char *) server->h_addr,
            (char *) &serv_addr.sin_addr.s_addr,
            server->h_length
    );
    serv_addr.sin_port = htons(atoi(argv[2]));

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        perror("Error creating socket");
        return 3;
    }

    // CONNECT TO THE SERVER
    if (connect(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
        perror("Error connecting to socket");
        return 4;
    }


//    if (pthread_create(&send_msg_thread, NULL, (void *) send_msg_handler, NULL) != 0) {
//        printf("ERROR: pthread\n");
//        return EXIT_FAILURE;
//    }
//
//    if (pthread_create(&recv_msg_thread, NULL, (void *) recv_msg_handler, NULL) != 0) {
//        printf("ERROR: pthread\n");
//        return EXIT_FAILURE;
//    }
//
//    n = write(sockfd, "jozko", NAME_LENGTH);

    // REGISTRACIA
    int resultUvodna = -1;
    int resultHlavna = -1;
    while (resultUvodna != 0) {
        if (resultHlavna == 0) {
            break;
        }
        resultUvodna = uvodnaObrazovka(buffer, sockfd, n);
//        if (resultUvodna != 0) {
//            /*if (pthread_create(&send_msg_thread, NULL, (void *) send_msg_handler, NULL) != 0) {
//                printf("ERROR: pthread\n");
//                return EXIT_FAILURE;
//            }*/
//
//            /*if (pthread_create(&recv_msg_thread, NULL, (void *) recv_msg_handler, NULL) != 0) {
//                printf("ERROR: pthread\n");
//                return EXIT_FAILURE;
//            }*/
//
//            int n = write(sockfd, name, NAME_LENGTH);
//        }
        if (resultUvodna == 1) {
            //koniec vrati 0, odhlasenie vrati 2
            while (resultHlavna != 2) {
                resultHlavna = hlavnaPonuka(buffer, sockfd, n);
                if (resultHlavna == 0) {
                    break;
                }
            }
        } else {
            continue;
        }
    }

    close(sockfd);

    return 0;
}

int registracia(char buffer[], int sockfd, int n) {
    char login[30], heslo[30], potvrdeneHeslo[30];
    bzero(buffer, 256);
    printf("\n\033[35;1mKLIENT: Zadajte login: \033[0m");
    scanf("%s", &login);
    getchar();
    printf("\n\033[35;1mKLIENT: Zadajte heslo: \033[0m");
    scanf("%s", &heslo);
    getchar();
    printf("\n\033[35;1mKLIENT: Potvrdte zadane heslo: \033[0m");
    scanf("%s", &potvrdeneHeslo);
    getchar();

    // Poskladanie spravy
    strcat(buffer, "registracia");
    strcat(buffer, " ");
    strcat(buffer, login);
    strcat(buffer, " ");
    strcat(buffer, heslo);
    strcat(buffer, " ");
    strcat(buffer, potvrdeneHeslo);

    buffer[strcspn(buffer, "\n")] = 0;

    // Poslanie udajov serveru
    writeToServer(buffer, sockfd);

    int vysledok = 0;

    // Odpoved zo servera
    listenToServer(buffer, sockfd);
    printf("%s\n", buffer);

    char *uspech;

    strtok(buffer, " ");
    uspech = strtok(NULL, " ");
    // ak bola registracia uspesna, uzivatel sa prihlasil a jeho login je poslany na server, ze je aktivny
    if (strcmp(uspech, "Uspesna") == 0) {
        vysledok = 1;
        strcat(name, "nickname");
        strcat(name, " ");
        strcat(name, login);
    }

    return vysledok;
}

int prihlasenie(char buffer[], int sockfd, int n) {
    char login[30], heslo[30];
    bzero(buffer, 256);
    printf("\n\033[35;1mKLIENT: Zadajte login: \033[0m");
    scanf("%s", &login);
    getchar();
    printf("\n\033[35;1mKLIENT: Zadajte heslo: \033[0m");
    scanf("%s", &heslo);
    getchar();

    // Poskladanie spravy
    strcat(buffer, "prihlasenie");
    strcat(buffer, " ");
    strcat(buffer, login);
    strcat(buffer, " ");
    strcat(buffer, heslo);

    buffer[strcspn(buffer, "\n")] = 0;

    // Poslanie udajov serveru
    writeToServer(buffer, sockfd);

    int vysledok = 0;
    // Odpoved zo servera
    listenToServer(buffer, sockfd);
    printf("%s\n", buffer);

    char *uspech;
    strtok(buffer, " ");
    uspech = strtok(NULL, " ");

    // uzivatel sa prihlasil a jeho login je poslany na server, ze je aktivny
    if (strcmp(uspech, "Uspesne") == 0) {
        vysledok = 1;
        strcat(name, "nickname");
        strcat(name, " ");
        strcat(name, login);
    }

    return vysledok;
}

int chatovanie(char buffer[], int sockfd) {
    char sprava[30];
    while (strcmp(sprava, "exit") != 0) {
        printf("\n\033[35;1mKLIENT: Prosim, zadajte spravu: \033[0m");
        bzero(buffer, 256);
        scanf("%[^\n]s", &sprava);
        getchar();

        strcat(buffer, "chatovanie");
        strcat(buffer, " ");
        strcat(buffer, sprava);
        buffer[strcspn(buffer, "\n")] = 0;

        //moja sprava je poslana na server
        writeToServer(buffer, sockfd);
        //server odpovie, ze spravu dostal
        listenToServer(buffer, sockfd);
        printf("%s\n", buffer);
    }
    return 0;
}

int zrusenieUctu(char buffer[], int sockfd, int n) {
    char login[30], heslo[30], potvrdeneHeslo[30];
    bzero(buffer, 256);
    printf("\n\033[35;1mKLIENT: Zadajte login: \033[0m");
    scanf("%s", &login);
    getchar();
    printf("\n\033[35;1mKLIENT: Zadajte heslo: \033[0m");
    scanf("%s", &heslo);
    getchar();

    // Poskladanie spravy
    strcat(buffer, "zrusenie");
    strcat(buffer, " ");
    strcat(buffer, login);
    strcat(buffer, " ");
    strcat(buffer, heslo);

    buffer[strcspn(buffer, "\n")] = 0;

    // Poslanie udajov serveru
    writeToServer(buffer, sockfd);

    int vysledok = 0;

    // Odpoved zo servera
    listenToServer(buffer, sockfd);
    printf("%s\n", buffer);

    char *uspech;

    strtok(buffer, " ");
    uspech = strtok(NULL, " ");

    if (strcmp(uspech, "Uspesne") == 0) {
        vysledok = 1;
    }

    return vysledok;
}

int uvodnaObrazovka(char buffer[], int sockfd, int n) {
    while (1) {
        int akcia = 0;
        puts("\033[36;1m|--- CHAT APP ---|\033[0m");
        puts("[1] Registracia");
        puts("[2] Prihlasenie");
        puts("[3] Zrusenie uctu");
        puts("[0] Koniec");
        printf("\n\033[35;1mKLIENT: Zadajte akciu: \033[0m");
        scanf("%d", &akcia);
        getchar();
        if (akcia == 1) {
            if (registracia(buffer, sockfd, n) == 1) {
                return 1;
            }
        } else if (akcia == 2) {
            if (prihlasenie(buffer, sockfd, n) == 1) {
                return 1;
            }
        } else if (akcia == 3) {
            zrusenieUctu(buffer, sockfd, n);
            return -1;
        } else if (akcia == 0) {
            n = write(sockfd, "exit", strlen("exit"));
            if (n < 0) {
                perror("Error writing to socket");
                return 5;
            }
            return 0;
        } else {
            printf("\n\033[35;1mKLIENT: Nespravne zvolena akcia!\033[0m");
        }
    }
    return -1;
}

int hlavnaPonuka(char buffer[], int sockfd, int n) {
    while (1) {
        int akcia = 0;
        puts("[1] Chatovanie");
        puts("[2] Odhlasenie");
        puts("[0] Koniec");
        printf("\n\033[35;1mKLIENT: Zadajte akciu: \033[0m");
        scanf("%d", &akcia);
        getchar();

        if (akcia == 1) {
            chatovanie(buffer, sockfd);
        } else if (akcia == 0) {
            n = write(sockfd, "exit", strlen("exit"));
            if (n < 0) {
                perror("Error writing to socket");
                return 5;
            }
            return 0;
        } else if (akcia == 2) {
            return 2;
        }
    }
}

void writeToServer(char *buffer, int sockfd) {
    int n = write(sockfd, buffer, strlen(buffer));
    if (n < 0) {
        perror("Error writing to socket");
    }
}

void listenToServer(char *buffer, int sockfd) {
    bzero(buffer, 256);
    int n = read(sockfd, buffer, 255);
    if (n < 0) {
        perror("Error reading from socket");
    }
}


