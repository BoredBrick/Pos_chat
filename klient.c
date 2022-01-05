//
// Created by Ma1túš Baláži on 28. 12. 2021.
//

#include "klient.h"


typedef struct data {
    int datasockfd;
    int *bolExit;
} D;

pthread_mutex_t mutexKlient = PTHREAD_MUTEX_INITIALIZER;

void *serverListener(void *data) {
    D *d = data ;

    char msgBuffer[BUFFER_SIZE];
    bzero(msgBuffer, BUFFER_SIZE);

    pthread_mutex_lock(&mutexKlient);
    while ((*d->bolExit) == 0) {
        pthread_mutex_unlock(&mutexKlient);

        if (recv(d->datasockfd, msgBuffer, BUFFER_SIZE, MSG_PEEK) > 0) {
            listenToSocket(msgBuffer, d->datasockfd);

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
            if (prebiehaSukromnyChat == 1) {
                akcia = 1;
            } else if (prebiehaSkupinovyChat == 1) {
                akcia = 2;
            } else {
                vypisHlavneMenu();
                scanf("%d", &akcia);
                getchar();
            }
        }

        if (akcia == 0) {
            if(jePrihlaseny) {
                for (int i = 0; i < KLIENTI_MAX_POCET; ++i) {
                    if (priatelia[i]) {
                        free(priatelia[i]);
                        priatelia[i] = NULL;
                    }
                }
                pocetPriatelov = 0;
            }
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
        usleep(500000);
    }

    return NULL;
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
    pthread_create(&vlaknoServera, NULL, &serverListener, &d);

    pthread_join(vlaknoKlienta, NULL);
    pthread_join(vlaknoServera, NULL);

//    for (int i = 0; i < KLIENTI_MAX_POCET; ++i) {
//        if (priatelia[i]) {
//            free(priatelia[i]);
//        }
//    }
    close(sockfd);

    return 0;
}






