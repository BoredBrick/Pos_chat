//
// Created by Matúš Baláži on 28. 12. 2021.
//



#ifndef POS_ZAPOCET_2_KLIENT_H
#define POS_ZAPOCET_2_KLIENT_H

#endif //POS_ZAPOCET_2_KLIENT_H


#include <malloc.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <pthread.h>
#include "vypisy.h"

char menoPrijemcuSpravy[LOGIN_MAX_DLZKA];
int jePrihlaseny = 0;
int prebiehaChat = 0;
int zacalChat = 0;


int registracia(int sockfd) {
    char login[LOGIN_MAX_DLZKA], heslo[HESLO_MAX_DLZKA], potvrdeneHeslo[HESLO_MAX_DLZKA], buffer[BUFFER_SIZE];
    bzero(buffer, BUFFER_SIZE);
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
    strcat(buffer, REGISTRACIA);
    strcat(buffer, " ");
    strcat(buffer, login);
    strcat(buffer, " ");
    strcat(buffer, heslo);
    strcat(buffer, " ");
    strcat(buffer, potvrdeneHeslo);

    buffer[strcspn(buffer, "\n")] = 0;

    strcpy(name, login);

    sifrujRetazec(buffer, buffer);

    // Poslanie udajov serveru
    writeToServer(buffer, sockfd);
}

int prihlasenie(int sockfd) {

    char login[LOGIN_MAX_DLZKA], heslo[HESLO_MAX_DLZKA], buffer[BUFFER_SIZE];
    bzero(buffer, BUFFER_SIZE);
    printf("\n\033[35;1mKLIENT: Zadajte login: \033[0m");
    scanf("%s", &login);
    getchar();
    printf("\n\033[35;1mKLIENT: Zadajte heslo: \033[0m");
    scanf("%s", &heslo);
    getchar();

    // Poskladanie spravy
    strcat(buffer, PRIHLASENIE);
    strcat(buffer, " ");
    strcat(buffer, login);
    strcat(buffer, " ");
    strcat(buffer, heslo);

    buffer[strcspn(buffer, "\n")] = 0;

    strcpy(name, login);

    sifrujRetazec(buffer, buffer);

    // Poslanie udajov serveru
    writeToServer(buffer, sockfd);
}

int chatovanie(char *meno, int sockfd) {
    char sprava[SPRAVA_MAX_DLZKA], buffer[BUFFER_SIZE];

    printf("\n\033[35;1mKLIENT: Prosim, zadajte spravu: \033[0m");
    bzero(buffer, BUFFER_SIZE);
    scanf("%[^\n]s", &sprava);
    getchar();

    if (strcmp(sprava, "exit") == 0) {
        strcat(buffer, UKONCENIE_CHATOVANIA);
    } else {
        strcat(buffer, CHATOVANIE);
    }

    strcat(buffer, " ");
    strcat(buffer, name);
    strcat(buffer, " ");
    strcat(buffer, meno);
    strcat(buffer, " ");
    strcat(buffer, sprava); // sprava obsahuje meno komu je urcena - admin toto je text spravy
    buffer[strcspn(buffer, "\n")] = 0;
    //chatovanie mojeMeno Admin text spravy
    //moja sprava je poslana na server

    sifrujRetazec(buffer, buffer);

    writeToServer(buffer, sockfd);

    if (strcmp(sprava, "exit") == 0) {
        return 0;
    } else {
        return 1;
    }
}

int zrusenieUctu(int sockfd) {
    char login[LOGIN_MAX_DLZKA], heslo[HESLO_MAX_DLZKA], buffer[BUFFER_SIZE];
    bzero(buffer, BUFFER_SIZE);
    printf("\n\033[35;1mKLIENT: Zadajte login: \033[0m");
    scanf("%s", &login);
    getchar();
    printf("\n\033[35;1mKLIENT: Zadajte heslo: \033[0m");
    scanf("%s", &heslo);
    getchar();

    // Poskladanie spravy
    strcat(buffer, ZRUSENIE_UCTU);
    strcat(buffer, " ");
    strcat(buffer, login);
    strcat(buffer, " ");
    strcat(buffer, heslo);

    buffer[strcspn(buffer, "\n")] = 0;

    sifrujRetazec(buffer, buffer);

    // Poslanie udajov serveru
    writeToServer(buffer, sockfd);
}

int ukoncenieAplikacie(int sockfd) {
    char buffer[BUFFER_SIZE];
    bzero(buffer, BUFFER_SIZE);
    strcat(buffer, "exit");
    sifrujRetazec(buffer, buffer);
    writeToServer(buffer, sockfd);
}

void onlineUzivatelia(int sockfd) {
    char buffer[BUFFER_SIZE];
    bzero(buffer, BUFFER_SIZE);
    strcat(buffer, ONLINE_UZIVATELIA);

    sifrujRetazec(buffer, buffer);

    // Poslanie udajov serveru
    writeToServer(buffer, sockfd);
}


void spracujPrikazZoServera(char *prikaz) {


    if (strcmp(prikaz, NEUSPESNA_REGISTRACIA) == 0) {
        puts("\n\033[31;1mKLIENT: Registracia bola neuspesna!\033[0m\n");

    } else if (strcmp(prikaz, USPESNA_REGISTRACIA) == 0) {
        jePrihlaseny = 1;
        puts("\n\033[32;1mKLIENT: Registracia prebehla uspesne!\033[0m\n");

    } else if (strcmp(prikaz, NEUSPESNE_PRIHLASENIE) == 0) {
        puts("\n\033[31;1mKLIENT: Prihlasenie bolo neuspesne!\033[0m\n");

    } else if (strcmp(prikaz, USPESNE_PRIHLASENIE) == 0) {
        jePrihlaseny = 1;
        puts("\n\033[32;1mKLIENT: Prihlasenie prebehlo uspesne!\033[0m\n");

    } else if (strcmp(prikaz, NEUSPESNE_ZRUSENIE) == 0) {
        puts("\n\033[31;1mKLIENT: Zrusenie uctu bolo neuspesne!\033[0m\n");

    } else if (strcmp(prikaz, USPESNE_ZRUSENIE) == 0) {
        puts("\n\033[32;1mKLIENT: Zrusenie uctu prebehlo uspesne!\033[0m\n");

    } else if (strcmp(prikaz, SPRAVA_ODOSIELATELOVI) == 0) {
        puts("\n\033[32;1mKLIENT: Sprava bola prijata serverom na spracovanie!\033[0m\n");

    } else if (strcmp(prikaz, SPRAVA_PRIJIMATELOVI) == 0) {
        vypisanieNovejSpravy();

    } else if (strcmp(prikaz, ZOZNAM_ONLINE_UZIVATELOV) == 0) {
        vypisOnlineUzivatelov();

    } else if (strcmp(prikaz, PRIDANIE_PRIATELA) == 0) {
        vypisPridaniePriatelaServer();

    } else if (strcmp(prikaz, ODSTRANENIE_PRIATELA) == 0) {

        vypisOdstraneniePriatelaServer();
    } else if (strcmp(prikaz, NEMOZNO_SPRIATELIT) == 0) {
        char *meno = strtok(NULL, "\0");
        printf("MENO: %s\n", meno);
        odstranPriatela(meno);
        puts("\n\033[31;1mKLIENT: Pridanie priatela bolo neuspesne!\033[0m\n");

    } else if (strcmp(prikaz, NEZRUSENIE_PRIATELSTVA) == 0) {
        char *meno = strtok(NULL, "\0");
        printf("MENO: %s\n", meno);
        pridajPriatela(meno);
        puts("\n\033[31;1mKLIENT: Zrusenie priatelstva bolo neuspesne!\033[0m\n");
    } else {
        printf("\n\033[31;1mKlient prijal neznamy prikaz od serveru:\033[0m %s\n", prikaz);
    }
}

const char *spracujUzivatelovuAkciu(int akcia, int sockfd) {

    if (!jePrihlaseny) {

        if (akcia == 1) {
            prihlasenie(sockfd);
            nacitajZoznamPriatelov();

        } else if (akcia == 2) {
            registracia(sockfd);
            nacitajZoznamPriatelov();

        } else if (akcia == 3) {
            zrusenieUctu(sockfd);
        } else {
            printf("\n\033[31;1mNespravne cislo akcie!\033[0m\n\n");
            return BREAK;
        }

    } else {
        if (akcia == 1) {

            if (zacalChat == 0) {
                bzero(menoPrijemcuSpravy, LOGIN_MAX_DLZKA);
                printf("\n\033[35;1mKLIENT: Zadajte meno pouzivatela, s ktorym chcete zacat konverzaciu: \033[0m");
                scanf("%s", &menoPrijemcuSpravy);
                getchar();

                if (jePriatel(menoPrijemcuSpravy) == 1) {
                    zacalChat = 1;
                    prebiehaChat = chatovanie(menoPrijemcuSpravy, sockfd);
                } else {
                    printf("\n\033[31;1mKLIENT: Zadany pouzivatel nie je vo Vasom zozname priatelov.\033[0m\n");
                    return BREAK;
                }

            } else {
                prebiehaChat = chatovanie(menoPrijemcuSpravy, sockfd);
            }
            if (prebiehaChat == 0) {
                zacalChat = 0;
                return BREAK;
            }
        }  else if (akcia == 2) {
            jePrihlaseny = 0;
            for (int i = 0; i < KLIENTI_MAX_POCET; ++i) {
                if (priatelia[i]) {
                    free(priatelia[i]);
                    priatelia[i] = NULL;
                }
            }
            pocetPriatelov = 0;

            return BREAK;

        } else if (akcia == 3) {
            onlineUzivatelia(sockfd);
        } else if (akcia == 4) {
            pridaniePriatelaKlient(sockfd);
            return BREAK;

        } else if (akcia == 5) {
            odstraneniePriatelaKlient(sockfd);
            return BREAK;

        } else if (akcia == 6) {
            vypisZoznamPriatelov();
            return BREAK;
        } else {
            printf("\n\033[31;1mNespravne cislo akcie!\033[0m\n\n");
            return BREAK;
        }
    }
    return "ok";
}


