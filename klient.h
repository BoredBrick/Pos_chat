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

char historiaSprav[BUFFER_HISTORIA_SPRAV_SIZE];
char menoPrijemcuSpravy[LOGIN_MAX_DLZKA];
char prijemcovia[BUFFER_SIZE];
int jePrihlaseny = 0;
int prebiehaSkupinovyChat = 0;
int prebiehaSukromnyChat = 0;

int zacalChat = 0;
int pocetPrijemcov = 0;
int nieJePriatel = 0;
int pocetSpravVBuffri = 0;

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

    strcpy(clientName, login);

    sifrujRetazec(buffer, buffer);

    // Poslanie udajov serveru
    writeToSocket(buffer, sockfd);
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

    strcpy(clientName, login);
    sifrujRetazec(buffer, buffer);

    // Poslanie udajov serveru
    writeToSocket(buffer, sockfd);
}

int sukromnyChat(char *meno, int sockfd) {
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
    strcat(buffer, clientName);
    strcat(buffer, " ");
    strcat(buffer, meno);
    strcat(buffer, " ");
    strcat(buffer, sprava); //
    buffer[strcspn(buffer, "\n")] = 0;

    sifrujRetazec(buffer, buffer);

    writeToSocket(buffer, sockfd);

    if (strcmp(sprava, "exit") == 0) {
        return 0;
    } else {
        pocetSpravVBuffri++;
        // odosielatel>prijimatel: obsah spravy\n
        strcat(historiaSprav, clientName);
        strcat(historiaSprav, ">");
        strcat(historiaSprav, meno);
        strcat(historiaSprav, ": ");
        strcat(historiaSprav, sprava);
        strcat(historiaSprav, "\n");

        return 1;
    }
}

int skupinovyChat(int pocet, char *prijemcovia, int sockfd) {
    char sprava[SPRAVA_MAX_DLZKA], buffer[BUFFER_SIZE];

    //prijemcovia POCET prijemca1 prijemca2
    printf("\n\033[35;1mKLIENT: Prosim, zadajte spravu: \033[0m");
    bzero(buffer, BUFFER_SIZE);
    bzero(sprava, SPRAVA_MAX_DLZKA);
    scanf("%[^\n]s", &sprava);
    getchar();

    char kopiaPrijemcov[BUFFER_SIZE];
    bzero(kopiaPrijemcov, BUFFER_SIZE);
    strcpy(kopiaPrijemcov, prijemcovia);
    char *ptr;

    for (int i = 0; i < pocet; ++i) {
        bzero(buffer, BUFFER_SIZE);

        if (strcmp(sprava, "exit") == 0) {
            strcat(buffer, UKONCENIE_CHATOVANIA);
        } else {
            strcat(buffer, SKUPINOVY_CHAT);
        }

        strcat(buffer, " ");
        strcat(buffer, clientName); //moje meno
        strcat(buffer, " ");
        if (i == 0) {
            strcat(buffer, strtok_r(kopiaPrijemcov, " ", &ptr));
        } else {
            strcat(buffer, strtok_r(NULL, " ", &ptr));
        }
        //strcat(buffer, strtok_r(prijemcovia," ",&ptr));

        strcat(buffer, " ");
        strcat(buffer, sprava);
        buffer[strcspn(buffer, "\n")] = 0;

        sifrujRetazec(buffer, buffer);

        writeToSocket(buffer, sockfd);

        // zapisovanie na socket trva dlhsie ako jedna iteracia cyklu, preto musime pockat
        sleep(1);
    }


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
    writeToSocket(buffer, sockfd);
}

int ukoncenieAplikacie(int sockfd) {
    char buffer[BUFFER_SIZE];
    bzero(buffer, BUFFER_SIZE);
    strcat(buffer, "exit");
    sifrujRetazec(buffer, buffer);
    writeToSocket(buffer, sockfd);
}

void onlineUzivatelia(int sockfd) {
    char buffer[BUFFER_SIZE];
    bzero(buffer, BUFFER_SIZE);
    strcat(buffer, ONLINE_UZIVATELIA);

    sifrujRetazec(buffer, buffer);

    // Poslanie udajov serveru
    writeToSocket(buffer, sockfd);
}

void vypisHistoriuChatu(char *menoPrijemcu) {
    char kopiaHistorieSprav[BUFFER_HISTORIA_SPRAV_SIZE];
    bzero(kopiaHistorieSprav, BUFFER_HISTORIA_SPRAV_SIZE);
    strcpy(kopiaHistorieSprav, historiaSprav);

    for (int i = 0; i < pocetSpravVBuffri; ++i) {
        char *odosielatel;
        char *prijimatel;
        char *sprava;
        char *ptr;

        if (i == 0) {
            odosielatel = strtok_r(kopiaHistorieSprav, ">", &ptr);
        } else {
            odosielatel = strtok_r(NULL, ">", &ptr);
        }
        prijimatel = strtok_r(NULL, ":", &ptr);
        sprava = strtok_r(NULL, "\n", &ptr);

        if ((strcmp(odosielatel, menoPrijemcu) == 0) || (strcmp(prijimatel, menoPrijemcu) == 0)) {
            vypisanieSpravyZHistorie(odosielatel, prijimatel, sprava);
        }
    }
}

void ulozPrijatuSpravuDoHistorie(char *kopiaSpravy) {
    strtok(kopiaSpravy, " ");

    char *odosielatel;
    odosielatel = strtok(NULL, " ");
    char *sprava;
    sprava = strtok(NULL, "\0");

    pocetSpravVBuffri++;

    // odosielatel>prijimatel: obsah spravy\n
    strcat(historiaSprav, odosielatel);
    strcat(historiaSprav, ">");
    strcat(historiaSprav, clientName);
    strcat(historiaSprav, ": ");
    strcat(historiaSprav, sprava);
}

void spracujPrikazZoServera(char *prikaz, char *kopiaSpravy) {

    if (strcmp(prikaz, NEUSPESNA_REGISTRACIA) == 0) {
        vypisNeuspesnaRegistracia();

    } else if (strcmp(prikaz, USPESNA_REGISTRACIA) == 0) {
        jePrihlaseny = 1;
        vypisUspesnaRegistracia();

    } else if (strcmp(prikaz, NEUSPESNE_PRIHLASENIE) == 0) {
        vypisNeuspesnePrihlasenie();

    } else if (strcmp(prikaz, USPESNE_PRIHLASENIE) == 0) {
        jePrihlaseny = 1;
        vypisUspesnePrihlasenie();

    } else if (strcmp(prikaz, NEUSPESNE_ZRUSENIE) == 0) {
        vypisNeuspesneZrusenie();

    } else if (strcmp(prikaz, USPESNE_ZRUSENIE) == 0) {
        vypisUspesneZrusenieUctu();

    } else if (strcmp(prikaz, SPRAVA_ODOSIELATELOVI) == 0) {
        vypisSpravaPrijataServerom();

    } else if (strcmp(prikaz, SPRAVA_PRIJIMATELOVI) == 0) {
        vypisNovejSpravy(prikaz);
        ulozPrijatuSpravuDoHistorie(kopiaSpravy);

    } else if ((strcmp(prikaz, SPRAVA_PRIJIMATELOVI_SKUPINA) == 0)) {
        vypisNovejSpravy(prikaz);

    } else if (strcmp(prikaz, ZOZNAM_ONLINE_UZIVATELOV) == 0) {
        vypisOnlineUzivatelov();

    } else if (strcmp(prikaz, PRIDANIE_PRIATELA) == 0) {
        vypisPridaniePriatela();

    } else if (strcmp(prikaz, ODSTRANENIE_PRIATELA) == 0) {
        vypisOdstraneniePriatelaServer();

    } else if (strcmp(prikaz, NEMOZNO_SPRIATELIT) == 0) {
        vypisNemoznoSpriatelit();

    } else if (strcmp(prikaz, NEZRUSENIE_PRIATELSTVA) == 0) {
        vypisNezruseniePriatelstva();

    } else {
        vypisNeznamyPrikaz(prikaz);
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
                    vypisHistoriuChatu(menoPrijemcuSpravy);
                    zacalChat = 1;
                    prebiehaSukromnyChat = sukromnyChat(menoPrijemcuSpravy, sockfd);
                } else {
                    printf("\n\033[31;1mKLIENT: Zadany pouzivatel nie je vo Vasom zozname priatelov.\033[0m\n");
                    return BREAK;
                }

            } else {
                vypisHistoriuChatu(menoPrijemcuSpravy);
                prebiehaSukromnyChat = sukromnyChat(menoPrijemcuSpravy, sockfd);
            }
            if (prebiehaSukromnyChat == 0) {
                zacalChat = 0;
                return BREAK;
            }

            // skupinovy chat
        } else if (akcia == 2) {
            if (zacalChat == 0) {
                nieJePriatel = 0;
                bzero(prijemcovia, BUFFER_SIZE);
                printf("\n\033[35;1mKLIENT: Zadajte pocet pouzivatelov, s ktorymi budete konverzovat: \033[0m");
                scanf("%d", &pocetPrijemcov);
                if (pocetPrijemcov >= 2) {
                    for (int i = 0; i < pocetPrijemcov; ++i) {
                        bzero(menoPrijemcuSpravy, LOGIN_MAX_DLZKA);
                        printf("\n\033[35;1mKLIENT: Zadajte meno %d. pouzivatela, s ktorym chcete zacat skupinovu konverzaciu: \033[0m",
                               (i + 1));
                        scanf("%s", &menoPrijemcuSpravy);
                        getchar();
                        strcat(prijemcovia, menoPrijemcuSpravy);
                        strcat(prijemcovia, " ");

                        if (jePriatel(menoPrijemcuSpravy) != 1) {
                            nieJePriatel = 1;
                            printf("\n\033[31;1mKLIENT: Zadany pouzivatel %s nie je vo Vasom zozname priatelov.\033[0m\n",
                                   menoPrijemcuSpravy);
                            break;
                        }
                    }
                    if (nieJePriatel == 0) {
                        zacalChat = 1;
                        prebiehaSkupinovyChat = skupinovyChat(pocetPrijemcov, prijemcovia, sockfd);
                    } else {
                        zacalChat = 0;
                        return BREAK;
                    }
                } else {
                    printf("\n\033[31;1mKLIENT: Na priamu komunikaciu s jednym priatelom pouzite sukromny chat.\033[0m\n");
                }
            } else {
                prebiehaSkupinovyChat = skupinovyChat(pocetPrijemcov, prijemcovia, sockfd);
            }
            if (prebiehaSkupinovyChat == 0) {
                zacalChat = 0;
                return BREAK;
            }

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

        } else if (akcia == 7) {
            jePrihlaseny = 0;
            freePolePriatelov();
            return BREAK;

        } else {
            printf("\n\033[31;1mNespravne cislo akcie!\033[0m\n\n");
            return BREAK;
        }
    }
    return "ok";
}


