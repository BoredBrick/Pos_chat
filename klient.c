//
// Created by Matúš Baláži on 28. 12. 2021.
//

#include "klient.h"
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>


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

    klientovCyklus(sockfd);

    //klientovCyklus2(sockfd);

    close(sockfd);

    return 0;
}






