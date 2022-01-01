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
#include <pthread.h>

#define BUFFER_SIZE 256
#define NAME_LENGTH 30

// volatile sig_atomic_t flag = 0;
int sockfd = 0;
char name[NAME_LENGTH];
int jePrihlaseny = 0;

//pthread_mutex_t klient_mutex = PTHREAD_MUTEX_INITIALIZER;

typedef struct data {
    int *bolExit;
} D;

//void *prijimanieSpravZoServera(void *data) {
//    D *d = data;
//    fd_set klientFD;
//    char msgBuffer[BUFFER_SIZE] = {};
//
//    //Resetovanie fd
//    FD_ZERO(&klientFD);
//    FD_SET(sockfd, &klientFD);
//    FD_SET(0, &klientFD);
//
//    //pthread_mutex_lock(&klient_mutex);
//    while ((*d->bolExit) == 0) {
//        //pthread_mutex_unlock(&klient_mutex);
//        //pthread_mutex_lock(&klient_mutex);
//
//        //if (select(FD_SETSIZE, &klientFD, NULL, NULL, NULL) != -1) //cakanie na volne fd
//        //{
//            for (int fd = 0; fd < FD_SETSIZE; fd++) {
//                if (FD_ISSET(fd, &klientFD)) {
//                    if (fd == sockfd) { //prijatie dat zo servera
//
//                        listenToServer(msgBuffer, sockfd);
//
//                        char *prikaz;
//                        char *odosielatel;
//                        char *sprava;
//
//                        prikaz = strtok(msgBuffer, " "); // vyseknutie prikazu zo serveru
//
//                        if (strcmp(prikaz, "REGFALSE") == 0) {
//
//                            puts("Registrácia bola neúspešná!");
//
//                        } else if (strcmp(prikaz, "REGTRUE") == 0) {
//                            jePrihlaseny = 1;
//                            puts("Registrácia prebehla úspešne!");
//
//                        } else if (strcmp(prikaz, "LOGINFALSE") == 0) {
//
//                            puts("Prihlásenie bolo neúspešné!");
//
//                        } else if (strcmp(prikaz, "LOGINTRUE") == 0) {
//                            jePrihlaseny = 1;
//                            puts("Prihlásenie prebehlo uspešne!");
//
//                        } else if (strcmp(prikaz, "SPRAVA") == 0) {
//                            puts("Sprava bola prijata!");
//
//                        } else if (strcmp(prikaz, "USER_SPRAVA") == 0) {
//                            odosielatel = strtok(NULL, " ");
//                            sprava = strtok(NULL, "\0");
//                            puts("Nova sprava od ineho klienta");
//                            printf("Nova sprava od %s : %s\n", odosielatel, sprava);
//                            fflush(stdout);
//
//                        }
//
//                        bzero(msgBuffer, BUFFER_SIZE);
//
//
//                        //pthread_mutex_unlock(&klient_mutex);
//                    }
//
//                    return NULL;
//                }
//            }
//        //}
//    }
//}
//
//
//void *posielanieSpravNaServer(void *data) {
//    D *d = data;
//    fd_set klientFD;
//    char buffer[BUFFER_SIZE] = {};
//
//    //Resetovanie fd
//    FD_ZERO(&klientFD);
//    FD_SET(sockfd, &klientFD);
//    FD_SET(0, &klientFD);
//
//    pthread_mutex_lock(&klient_mutex);
//    while ((*d->bolExit) == 0) {
//        pthread_mutex_unlock(&klient_mutex);
//        //pthread_mutex_lock(&klient_mutex);
//
//        //if (select(FD_SETSIZE, &klientFD, NULL, NULL, NULL) != -1) //cakanie na volne fd
//        //{
//            for (int fd = 0; fd < FD_SETSIZE; fd++) {
//                if (FD_ISSET(fd, &klientFD)) {
//                    if (fd == sockfd) { // poslanie dat na server
//
//
//                        int akcia = -1;
//                        if (!jePrihlaseny) {
//                            puts("\033[36;1m|--- CHAT APP ---|\033[0m");
//                            puts("[1] Registracia");
//                            puts("[2] Prihlasenie");
//                            puts("[3] Zrusenie uctu");
//                            puts("[0] Koniec");
//                            printf("\n\033[35;1mKLIENT: Zadajte akciu: \033[0m");
//                            scanf("%d", &akcia);
//                            getchar();
//                        } else {
//                            puts("[1] Posli spravu");
//                            puts("[2] Ziskaj spravu");
//                            puts("[3] Odhlasenie");
//                            puts("[0] Koniec");
//                            printf("\n\033[35;1mKLIENT: Zadajte akciu: \033[0m");
//                            scanf("%d", &akcia);
//                            getchar();
//                        }
//
//                        if (akcia == 0) {
//                            (*d->bolExit) = 1;
//                            writeToServer("exit", sockfd);
//                            break;
//                        }
//
//                        if (!jePrihlaseny) {
//
//                            if (akcia == 1) {
//                                char buffer[256];
//                                char login[30], heslo[30], potvrdeneHeslo[30];
//                                bzero(buffer, 256);
//                                printf("\n\033[35;1mKLIENT: Zadajte login: \033[0m");
//                                scanf("%s", &login);
//                                getchar();
//                                printf("\n\033[35;1mKLIENT: Zadajte heslo: \033[0m");
//                                scanf("%s", &heslo);
//                                getchar();
//                                printf("\n\033[35;1mKLIENT: Potvrdte zadane heslo: \033[0m");
//                                scanf("%s", &potvrdeneHeslo);
//                                getchar();
//
//                                // Poskladanie spravy
//                                strcat(buffer, "registracia");
//                                strcat(buffer, " ");
//                                strcat(buffer, login);
//                                strcat(buffer, " ");
//                                strcat(buffer, heslo);
//                                strcat(buffer, " ");
//                                strcat(buffer, potvrdeneHeslo);
//
//                                buffer[strcspn(buffer, "\n")] = 0;
//
//                                strcpy(name, login);
//
//                                // Poslanie udajov serveru
//                                writeToServer(buffer, sockfd);
//                            } else if (akcia == 2) {
//                                char buffer[256];
//                                char login[30], heslo[30];
//                                bzero(buffer, 256);
//                                printf("\n\033[35;1mKLIENT: Zadajte login: \033[0m");
//                                scanf("%s", &login);
//                                getchar();
//                                printf("\n\033[35;1mKLIENT: Zadajte heslo: \033[0m");
//                                scanf("%s", &heslo);
//                                getchar();
//
//                                // Poskladanie spravy
//                                strcat(buffer, "prihlasenie");
//                                strcat(buffer, " ");
//                                strcat(buffer, login);
//                                strcat(buffer, " ");
//                                strcat(buffer, heslo);
//
//                                buffer[strcspn(buffer, "\n")] = 0;
//
//                                strcpy(name, login);
//
//                                // Poslanie udajov serveru
//                                writeToServer(buffer, sockfd);
//
//                            }
//
//
//                        } else {
//                            if (akcia == 1) {
//                                char buffer[256];
//
//                                char sprava[30];
//
//                                printf("\n\033[35;1mKLIENT: Prosim, zadajte spravu: \033[0m");
//                                bzero(buffer, 256);
//                                scanf("%[^\n]s", &sprava);
//                                getchar();
//
//                                strcat(buffer, "chatovanie");
//                                strcat(buffer, " ");
//                                strcat(buffer, name);
//                                strcat(buffer, " ");
//                                strcat(buffer,
//                                       sprava); // sprava obsahuje meno komu je urcena - admin toto je text spravy
//                                buffer[strcspn(buffer, "\n")] = 0;
//                                //chatovanie mojeMeno Admin text spravy
//                                //moja sprava je poslana na server
//                                writeToServer(buffer, sockfd);
//
//                            } else if (akcia == 2) {
//                                continue;
//                            } else if (akcia == 3) {
//                                jePrihlaseny = 0;
//
//                            }
//                        }
//                    }
//
//
//                    //pthread_mutex_unlock(&klient_mutex);
//                }
//
//                return NULL;
//            }
//        //}
//    }
//}


void klientovCyklus(int sockfd) {
    fd_set klientFD;

    char sprava[BUFFER_SIZE];
    char msgBuffer[BUFFER_SIZE];
    char chatBuffer[BUFFER_SIZE];

    while (1) {
        int akcia = -1;
        if (!jePrihlaseny) {
            puts("\033[36;1m|--- CHAT APP ---|\033[0m");
            puts("[1] Registracia");
            puts("[2] Prihlasenie");
            puts("[3] Zrusenie uctu");
            puts("[0] Koniec");
            printf("\n\033[35;1mKLIENT: Zadajte akciu: \033[0m");
            scanf("%d", &akcia);
            getchar();
        } else {
            puts("[1] Posli spravu");
            puts("[2] Ziskaj spravu");
            puts("[3] Odhlasenie");
            puts("[0] Koniec");
            printf("\n\033[35;1mKLIENT: Zadajte akciu: \033[0m");
            scanf("%d", &akcia);
            getchar();
        }

        if (akcia == 0) {
            writeToServer("exit", sockfd);
            break;
        }

        //Resetovanie fd
        FD_ZERO(&klientFD);
        FD_SET(sockfd, &klientFD);
        FD_SET(0, &klientFD);

        //if (select(FD_SETSIZE, &klientFD, NULL, NULL, NULL) != -1) //cakanie na volne fd
        //{
        for (int fd = 0; fd < FD_SETSIZE; fd++) {
            if (FD_ISSET(fd, &klientFD)) {
                if (fd == sockfd) { //prijatie dat zo servera
                    // spravy od servera


                    listenToServer(msgBuffer, sockfd);

                    char *prikaz;
                    char *odosielatel;
                    char *sprava;

                    prikaz = strtok(msgBuffer, " "); // vyseknutie prikazu zo serveru

                    if (strcmp(prikaz, "REGFALSE") == 0) {

                        puts("Registrácia bola neúspešná!");

                    } else if (strcmp(prikaz, "REGTRUE") == 0) {
                        jePrihlaseny = 1;
                        puts("Registrácia prebehla úspešne!");

                    } else if (strcmp(prikaz, "LOGINFALSE") == 0) {

                        puts("Prihlásenie bolo neúspešné!");

                    } else if (strcmp(prikaz, "LOGINTRUE") == 0) {
                        jePrihlaseny = 1;
                        puts("Prihlásenie prebehlo uspešne!");

                    } else if (strcmp(prikaz, "SPRAVA") == 0) {
                        puts("Sprava bola prijata!");

                    } else if (strcmp(prikaz, "USER_SPRAVA") == 0) {
                        odosielatel = strtok(NULL, " ");
                        sprava = strtok(NULL, "\0");
                        puts("Nova sprava od ineho klienta");
                        printf("Nova sprava od %s : %s\n", odosielatel, sprava);
                        fflush(stdout);

                    }

                    bzero(msgBuffer, BUFFER_SIZE);
                } else if (fd == 0) { // citanie z klavesnice a posielanie na server
                    // spravy serveru
                    if (!jePrihlaseny) {

                        if (akcia == 1) {
                            char buffer[256];
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

                            strcpy(name, login);

                            // Poslanie udajov serveru
                            writeToServer(buffer, sockfd);
                        } else if (akcia == 2) {
                            char buffer[256];
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

                            strcpy(name, login);

                            // Poslanie udajov serveru
                            writeToServer(buffer, sockfd);

                        }

                    } else {
                        if (akcia == 1) {
                            char buffer[256];

                            char sprava[30];

                            printf("\n\033[35;1mKLIENT: Prosim, zadajte spravu: \033[0m");
                            bzero(buffer, 256);
                            scanf("%[^\n]s", &sprava);
                            getchar();

                            strcat(buffer, "chatovanie");
                            strcat(buffer, " ");
                            strcat(buffer, name);
                            strcat(buffer, " ");
                            strcat(buffer,
                                   sprava); // sprava obsahuje meno komu je urcena - admin toto je text spravy
                            buffer[strcspn(buffer, "\n")] = 0;
                            //chatovanie mojeMeno Admin text spravy
                            //moja sprava je poslana na server
                            writeToServer(buffer, sockfd);

                        } else if (akcia == 2) {
                            continue;
                        } else if (akcia == 3) {
                            jePrihlaseny = 0;

                        }
                    }
                }
            }
        }
        //}
    }
}

int main(int argc, char *argv[]) {
    setlinebuf(stdout);
    int sockfd, n;
    struct sockaddr_in serv_addr;
    struct hostent *server;
    char buffer[256];

    //pthread_t writeVlakno;
    //pthread_t readVlakno;


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

    //int bolExit = 0;
    //D d = {&bolExit};

//    pthread_create(&writeVlakno, NULL, &posielanieSpravNaServer, &d);
//    pthread_create(&readVlakno, NULL, &prijimanieSpravZoServera, &d);


    // pthread_join(writeVlakno, NULL);
    // pthread_join(readVlakno, NULL);

    klientovCyklus(sockfd);






    close(sockfd);

    return 0;
}


int registracia(char buffer[], int sockfd, int n) {

}

int prihlasenie(char buffer[], int sockfd, int n) {

}

int chatovanie(char buffer[], int sockfd) {

}

int zrusenieUctu(char buffer[], int sockfd, int n) {

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


