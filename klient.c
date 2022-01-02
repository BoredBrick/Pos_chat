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


int sockfd = 0;

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

    int prebiehaRegistracia = 0, prebiehaPrihlasenie = 0, prebiehaZrusenieUctu = 0, prebiehaChat = 0;

    char buffer[BUFFER_SIZE];
    char msgBuffer[BUFFER_SIZE];


    while (1) {
        int akcia = -1;
        if (!jePrihlaseny) {
            puts("\033[36;1m|--- CHAT APP ---|\033[0m");
            puts("[1] Prihlasenie");
            puts("[2] Registracia");
            puts("[3] Zrusenie uctu");
            puts("[0] Koniec");
            printf("\n\033[35;1mKLIENT: Zadajte akciu: \033[0m");
            scanf("%d", &akcia);
            getchar();
        } else {
            if (prebiehaChat == 0) {
                puts("\n[1] Posli spravu");
                puts("[2] Ziskaj spravu");
                puts("[3] Odhlasenie");
                puts("[4] Online uzivatelia");
                puts("[5] Pridaj priatela");
                puts("[6] Odstran priatela");
                puts("[7] Zoznam priatelov");
                puts("[0] Koniec");
                printf("\n\033[35;1mKLIENT: Zadajte akciu: \033[0m");
                scanf("%d", &akcia);
                getchar();
            } else {
                akcia = 1;
            }
        }

        if (akcia == 0) {
            ukoncenieAplikacie(buffer, sockfd);
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
                    char *odosielatel;
                    char *sprava;

                    prikaz = strtok(msgBuffer, " "); // vyseknutie prikazu zo serveru

                    if (strcmp(prikaz, NEUSPESNA_REGISTRACIA) == 0) {
                        puts("\n\033[35;1mKLIENT: Registracia bola neuspesna!\033[0m\n");

                    } else if (strcmp(prikaz, USPESNA_REGISTRACIA) == 0) {
                        jePrihlaseny = 1;
                        puts("\n\033[35;1mKLIENT: Registracia prebehla uspesne!\033[0m\n");

                    } else if (strcmp(prikaz, NEUSPESNE_PRIHLASENIE) == 0) {
                        puts("\n\033[35;1mKLIENT: Prihlasenie bolo neuspesne!\033[0m\n");

                    } else if (strcmp(prikaz, USPESNE_PRIHLASENIE) == 0) {
                        jePrihlaseny = 1;
                        puts("\n\033[35;1mKLIENT: Prihlasenie prebehlo uspesne!\033[0m\n");

                    } else if (strcmp(prikaz, NEUSPESNE_ZRUSENIE) == 0) {
                        puts("\n\033[35;1mKLIENT: Zrusenie uctu bolo neuspesne!\033[0m\n");

                    } else if (strcmp(prikaz, USPESNE_ZRUSENIE) == 0) {
                        puts("\n\033[35;1mKLIENT: Zrusenie uctu prebehlo uspesne!\033[0m\n");

                    } else if (strcmp(prikaz, SPRAVA_ODOSIELATELOVI) == 0) {
                        puts("\n\033[35;1mKLIENT: Sprava bola prijata serverom na spracovanie!\033[0m\n");

                    } else if (strcmp(prikaz, SPRAVA_PRIJIMATELOVI) == 0) {
                        odosielatel = strtok(NULL, " ");
                        sprava = strtok(NULL, "\0");
                        puts("Nova sprava od ineho klienta");
                        printf("Nova sprava od %s: %s\n", odosielatel, sprava);
                        fflush(stdout);

                    } else if (strcmp(prikaz, ZOZNAM_ONLINE_UZIVATELOV) == 0) {
                        int pocetOnline = atoi(strtok(NULL, " "));
                        if (pocetOnline <= 1) {
                            printf("Okrem vas nie je online ziadny iny pouzivatel\n");
                            break;
                        } else {
                            printf("Nasledujuci pouzivatelia su online: \n");
                        }
                        for (int i = 0; i < pocetOnline; ++i) {
                            char *meno = strtok(NULL, " ");
                            if (strcmp(meno, name) == 0) {
                                printf("Vy: %s\n", meno);
                            } else {
                                printf("%s \n", meno);
                            }
                        }
                    } else if (strcmp(prikaz, PRIDANIE_PRIATELA) == 0) {
                        char *ziadostOd;
                        ziadostOd = strtok(NULL, "\0");
                        pridajPriatela(ziadostOd);
                        printf("\n\033[35;1mKLIENT: Pouzivatel %s si Vas pridal do priatelov!\033[0m\n", ziadostOd);

                    } else if (strcmp(prikaz, ODSTRANENIE_PRIATELA) == 0) {
                        char *ziadostOd;
                        ziadostOd = strtok(NULL, "\0");
                        odstranPriatela(ziadostOd);
                        printf("\n\033[35;1mKLIENT: Pouzivatel %s si Vas odstranil z priatelov!\033[0m\n", ziadostOd);

                    }

                    bzero(msgBuffer, BUFFER_SIZE);
                } else if (fd == 0) { // citanie z klavesnice a posielanie na server
                    // spravy serveru
                    if (!jePrihlaseny) {

                        if (akcia == 1) {
                            prebiehaPrihlasenie = prihlasenie(buffer, sockfd);
                            nacitajZoznamPriatelov();

                        } else if (akcia == 2) {
                            prebiehaRegistracia = registracia(buffer, sockfd);
                            nacitajZoznamPriatelov();

                        } else if (akcia == 3) {
                            prebiehaZrusenieUctu = zrusenieUctu(buffer, sockfd);
                        }

                    } else {
                        if (akcia == 1) {
                            prebiehaChat = chatovanie(buffer, sockfd);
                            if (prebiehaChat == 0) {
                                break;
                            }
                        } else if (akcia == 2) {
                            continue;

                        } else if (akcia == 3) {
                            jePrihlaseny = 0;
                            break;

                        } else if (akcia == 4) {
                            onlineUzivatelia(sockfd);
                        } else if (akcia == 5) {
                            char menoPridavaneho[LOGIN_MAX_DLZKA];
                            bzero(menoPridavaneho, LOGIN_MAX_DLZKA);
                            printf("\n\033[35;1mKLIENT: Zadajte meno pouzivatela, ktoreho chcete pridat do priatelov: \033[0m");
                            scanf("%s", &menoPridavaneho);
                            getchar();
                            pridajPriatela(menoPridavaneho);
                            posliZiadostOPriatelstvo(sockfd, menoPridavaneho);
                            break;

                        } else if (akcia == 6) {
                            char menoOdstranovaneho[LOGIN_MAX_DLZKA];
                            bzero(menoOdstranovaneho, LOGIN_MAX_DLZKA);
                            printf("\n\033[35;1mKLIENT: Zadajte meno pouzivatela, ktoreho chcete odstranit z priatelov: \033[0m");
                            scanf("%s", &menoOdstranovaneho);
                            getchar();
                            odstranPriatela(menoOdstranovaneho);
                            posliInfoOOdstraneniZPriatelov(sockfd, menoOdstranovaneho);
                            break;

                        } else if (akcia == 7) {
                            vypisZoznamPriatelov();
                            break;
                        } else {
                            printf("Nespravne cislo akcie!\n");
                        }
                    }
                    bzero(buffer, BUFFER_SIZE);
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






