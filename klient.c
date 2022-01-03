//
// Created by Ma1túš Baláži on 28. 12. 2021.
//

#include "klient.h"
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

void *serverPocuva(void *data) {
    D *d = data;

    char msgBuffer[BUFFER_SIZE];
    bzero(msgBuffer, BUFFER_SIZE);

    pthread_mutex_lock(&mutexKlient);
    while ((*d->bolExit) == 0) {
        pthread_mutex_unlock(&mutexKlient);

        if (recv(d->datasockfd, msgBuffer, BUFFER_SIZE, MSG_PEEK) > 0) {
            listenToServer(msgBuffer, d->datasockfd);

            odsifrujRetazec(msgBuffer, msgBuffer);

            char *prikaz;
            prikaz = strtok(msgBuffer, " "); // vyseknutie prikazu zo serveru
            spracujPrikazZoServera(prikaz);

            bzero(msgBuffer, BUFFER_SIZE);
        }

    }

    return NULL;
}

void *klientZapisuje(void *data) {
    D *d = data;

    pthread_mutex_lock(&mutexKlient);
    while ((*d->bolExit) == 0) {
        pthread_mutex_unlock(&mutexKlient);

        int akcia = -1;
        if (!jePrihlaseny) {
            vypisUvodneMenu();
            scanf("%d", &akcia);
            getchar();
        } else {
            if (prebiehaChat == 0) {
                vypisHlavneMenu();
                scanf("%d", &akcia);
                getchar();
            } else {
                akcia = 1;
            }
        }

        if (akcia == 0) {
            (*d->bolExit) = 1;
            ukoncenieAplikacie(d->datasockfd);
            break;
        }
        spracujUzivatelovuAkciu(akcia, d->datasockfd);
//        const char *res = spracujUzivatelovuAkciu(akcia, d->datasockfd);
//        if (strcmp(res, BREAK) == 0) {
//            break;
//        } else if (strcmp(res, CONTINUE) == 0) {
//            continue;
//        }
        sleep(1);
    }

    return NULL;
}

void klientovCyklus2(int sockfd) {

    int prebiehaRegistracia = 0, prebiehaPrihlasenie = 0, prebiehaChat = 0;

    char msgBuffer[BUFFER_SIZE];

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
            if (prebiehaChat == 0) {
                puts("[1] Posli spravu");
                puts("[2] Ziskaj spravu");
                puts("[3] Odhlasenie");
                puts("[0] Koniec");
                printf("\n\033[35;1mKLIENT: Zadajte akciu: \033[0m");
                scanf("%d", &akcia);
                getchar();
            } else {
                akcia = 1;
            }
        }

        if (akcia == 0) {
            writeToServer("exit", sockfd);
            break;
        }

        // citanie z klavesnice a posielanie na server
        // spravy serveru
        if (!jePrihlaseny) {

            if (akcia == 1) {
                char buffer[256];
                prebiehaRegistracia = registracia2(buffer, sockfd);

            } else if (akcia == 2) {
                char buffer[256];
                prebiehaPrihlasenie = prihlasenie2(buffer, sockfd);

            }

        } else {
            if (akcia == 1) {
                char buffer[256];
                prebiehaChat = chatovanie2(buffer, sockfd);

            } else if (akcia == 2) {
                continue;

            } else if (akcia == 3) {
                jePrihlaseny = 0;

            }
        }
    }
}

void klientovCyklus(int sockfd) {
    fd_set klientFD;

    char msgBuffer[BUFFER_SIZE];

    while (1) {
        int akcia = -1;
        if (!jePrihlaseny) {
            vypisUvodneMenu();
            scanf("%d", &akcia);
            getchar();
        } else {
            if (prebiehaChat == 0) {
                vypisHlavneMenu();
                scanf("%d", &akcia);
                getchar();
            } else {
                akcia = 1;
            }
        }

        if (akcia == 0) {
            ukoncenieAplikacie(sockfd);
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

                    odsifrujRetazec(msgBuffer, msgBuffer);

                    char *prikaz;
                    prikaz = strtok(msgBuffer, " "); // vyseknutie prikazu zo serveru
                    spracujPrikazZoServera(prikaz);

                    bzero(msgBuffer, BUFFER_SIZE);
                } else if (fd == 0) { // citanie z klavesnice a posielanie na server
                    // spravy serveru
                    const char *res = spracujUzivatelovuAkciu(akcia, sockfd);
                    if (strcmp(res, BREAK) == 0) {
                        break;
                    } else if (strcmp(res, CONTINUE) == 0) {
                        continue;
                    }
                }
            }
        }
        //}
    }
}


int main(int argc, char *argv[]) {
    setlinebuf(stdout);
    int sockfd;
    struct sockaddr_in serv_addr;
    struct hostent *server;

    pthread_t vlaknoKlienta, vlaknoServera;

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

    int datasockfd = sockfd;
    int bolExit = 0;
    D d = {datasockfd, &bolExit};

    pthread_create(&vlaknoKlienta, NULL, &klientZapisuje, &d);
    pthread_create(&vlaknoServera, NULL, &serverPocuva, &d);

    pthread_join(vlaknoKlienta, NULL);
    pthread_join(vlaknoServera, NULL);

    //klientovCyklus(sockfd);

    //klientovCyklus2(sockfd);

    for (int i = 0; i < KLIENTI_MAX_POCET; ++i) {
        if (priatelia[i]) {
           free(priatelia[i]);
        }
    }
    close(sockfd);

    return 0;
}






