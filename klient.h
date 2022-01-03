//
// Created by Matúš Baláži on 28. 12. 2021.
//

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <malloc.h>
#include <stdlib.h>
#include "procedury.h"

#ifndef POS_ZAPOCET_2_KLIENT_H
#define POS_ZAPOCET_2_KLIENT_H

#endif //POS_ZAPOCET_2_KLIENT_H

char name[LOGIN_MAX_DLZKA];
char menoPrijemcuSpravy[LOGIN_MAX_DLZKA];
int jePrihlaseny = 0;
int pocetPriatelov = 0;
int prebiehaChat = 0;
int zacalChat = 0;

typedef struct {
    char name[LOGIN_MAX_DLZKA];
} priatel;
priatel *priatelia[MAX_POCET_PRIATELOV];

void writeToServer(char *buffer, int sockfd) {
    int n = write(sockfd, buffer, strlen(buffer));
    if (n < 0) {
        perror("Error writing to socket");
    }
}

void listenToServer(char *buffer, int sockfd) {
    bzero(buffer, BUFFER_SIZE);
    int n = read(sockfd, buffer, BUFFER_SIZE - 1);
    if (n < 0) {
        perror("Error reading from socket");
    }
}

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

int registracia2(char buffer[], int sockfd) {
    char login[LOGIN_MAX_DLZKA], heslo[HESLO_MAX_DLZKA], potvrdeneHeslo[HESLO_MAX_DLZKA];
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

    listenToServer(buffer, sockfd);

    char *prikaz;
    char *odosielatel;
    char *sprava;

    prikaz = strtok(buffer, " ");

    if (strcmp(prikaz, NEUSPESNA_REGISTRACIA) == 0) {

        puts("Registrácia bola neúspešná!");

    } else if (strcmp(prikaz, USPESNA_REGISTRACIA) == 0) {
        jePrihlaseny = 1;
        puts("Registrácia prebehla úspešne!");

    }

    bzero(buffer, BUFFER_SIZE);
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

int prihlasenie2(char buffer[], int sockfd) {

    char login[LOGIN_MAX_DLZKA], heslo[HESLO_MAX_DLZKA];
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

    listenToServer(buffer, sockfd);

    char *prikaz;
    char *odosielatel;
    char *sprava;

    prikaz = strtok(buffer, " ");

    if (strcmp(prikaz, NEUSPESNE_PRIHLASENIE) == 0) {
        puts("Prihlásenie bolo neúspešné!");

    } else if (strcmp(prikaz, USPESNE_PRIHLASENIE) == 0) {
        jePrihlaseny = 1;
        puts("Prihlásenie prebehlo uspešne!");

    }

    bzero(buffer, BUFFER_SIZE);
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

int chatovanie2(char buffer[], int sockfd) {
    char sprava[SPRAVA_MAX_DLZKA];

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
    strcat(buffer, sprava); // sprava obsahuje meno komu je urcena - admin toto je text spravy
    buffer[strcspn(buffer, "\n")] = 0;
    //chatovanie mojeMeno Admin text spravy
    //moja sprava je poslana na server

    sifrujRetazec(buffer, buffer);

    writeToServer(buffer, sockfd);

    listenToServer(buffer, sockfd);

    char *prikaz;
    char *odosielatel;
    char *sprava2;

    prikaz = strtok(buffer, " ");

    if (strcmp(prikaz, SPRAVA_ODOSIELATELOVI) == 0) {
        puts("Sprava bola prijata!");

    } else if (strcmp(prikaz, SPRAVA_PRIJIMATELOVI) == 0) {
        odosielatel = strtok(NULL, " ");
        sprava2 = strtok(NULL, "\0");
        puts("Nova sprava od ineho klienta");
        printf("Nova sprava od %s: %s\n", odosielatel, sprava2);
        fflush(stdout);
    }

    bzero(buffer, BUFFER_SIZE);

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

void klientovCyklus(int sockfd);

void klientovCyklus2(int sockfd);

void onlineUzivatelia(int sockfd) {
    char buffer[BUFFER_SIZE];
    bzero(buffer, BUFFER_SIZE);
    strcat(buffer, ONLINE_UZIVATELIA);

    sifrujRetazec(buffer, buffer);

    // Poslanie udajov serveru
    writeToServer(buffer, sockfd);
}

void posliZiadostOPriatelstvo(int sockfd, char *komu) {
    char buffer[BUFFER_SIZE];
    bzero(buffer, BUFFER_SIZE);
    strcat(buffer, NOVY_PRIATEL);
    strcat(buffer, " ");
    strcat(buffer, komu);
    strcat(buffer, " ");
    strcat(buffer, name);

    sifrujRetazec(buffer, buffer);

    // Poslanie udajov serveru
    writeToServer(buffer, sockfd);
}

void posliInfoOOdstraneniZPriatelov(int sockfd, char *komu) {
    char buffer[BUFFER_SIZE];
    bzero(buffer, BUFFER_SIZE);
    strcat(buffer, ZRUS_PRIATELA);
    strcat(buffer, " ");
    strcat(buffer, komu);
    strcat(buffer, " ");
    strcat(buffer, name);

    sifrujRetazec(buffer, buffer);

    // Poslanie udajov serveru
    writeToServer(buffer, sockfd);
}

void pridajDoPolaPriatelov(priatel *priatel) {
    for (int i = 0; i < KLIENTI_MAX_POCET; i++) {
        if (!priatelia[i]) {
            priatelia[i] = priatel;
            pocetPriatelov++;
            break;
        }
    }
}

void odstranZPolaPriatelov(char *meno) {
    for (int i = 0; i < KLIENTI_MAX_POCET; ++i) {
        if (priatelia[i]) {
            if (strcmp(priatelia[i]->name, meno) == 0) {
                free(priatelia[i]);
                priatelia[i] = NULL;
                pocetPriatelov--;
                break;
            }
        }
    }
}

void nacitajZoznamPriatelov() {
    FILE *suborPriatelia, *suborZaregistrovani;

    char nazovSuboru[BUFFER_SIZE];
    bzero(nazovSuboru, BUFFER_SIZE);
    strcat(nazovSuboru, name);
    strcat(nazovSuboru, ".txt");

    suborPriatelia = fopen(nazovSuboru, "a+");
    suborZaregistrovani = fopen(MENO_SUBORU, "a+");

    char loginPriatelia[LOGIN_MAX_DLZKA];
    char loginRegistrovani[LOGIN_MAX_DLZKA];
    bzero(loginRegistrovani, LOGIN_MAX_DLZKA);
    bzero(loginPriatelia, LOGIN_MAX_DLZKA);


    while (fscanf(suborPriatelia, " %s", loginPriatelia) == 1) {
        while (fscanf(suborZaregistrovani, " %s", loginRegistrovani) == 1) {
            if (strcmp(loginPriatelia, loginRegistrovani) == 0) {
                priatel *novyPriatel = (priatel *) malloc(sizeof(priatel));
                strcpy(novyPriatel->name, loginPriatelia);
                pridajDoPolaPriatelov(novyPriatel);
                bzero(loginRegistrovani, LOGIN_MAX_DLZKA);
                break;

            }
            bzero(loginRegistrovani, LOGIN_MAX_DLZKA);
        }
        bzero(loginPriatelia, LOGIN_MAX_DLZKA);
        rewind(suborZaregistrovani);
    }

    fclose(suborPriatelia);
    fclose(suborZaregistrovani);
}

void pridajPriatela(char *menoPridavaneho) {
    FILE *suborPriatelia;

    char nazovSuboru[BUFFER_SIZE];
    bzero(nazovSuboru, BUFFER_SIZE);
    strcat(nazovSuboru, name);
    strcat(nazovSuboru, ".txt");

    suborPriatelia = fopen(nazovSuboru, "a+");

    int uzJeVPriateloch = 0;
    char loginPriatelia[LOGIN_MAX_DLZKA];

    while (fscanf(suborPriatelia, " %s", loginPriatelia) == 1) {
        if (strcmp(loginPriatelia, menoPridavaneho) == 0) {
            uzJeVPriateloch = 1;
            break;
        }
    }

    if (!uzJeVPriateloch) {
        priatel *novyPriatel = (priatel *) malloc(sizeof(priatel));
        strcpy(novyPriatel->name, menoPridavaneho);
        pridajDoPolaPriatelov(novyPriatel);
        fprintf(suborPriatelia, menoPridavaneho);
        fprintf(suborPriatelia, "\n");
    }

    fclose(suborPriatelia);

}

void odstranPriatela(char *menoOdstranovaneho) {
    FILE *suborPriatelia, *novySubor;

    char nazovSuboru[BUFFER_SIZE];
    bzero(nazovSuboru, BUFFER_SIZE);
    strcat(nazovSuboru, name);
    strcat(nazovSuboru, ".txt");

    suborPriatelia = fopen(nazovSuboru, "a+");

    int jeVPriateloch = 0;
    char loginPriatelia[LOGIN_MAX_DLZKA];

    while (fscanf(suborPriatelia, " %s", loginPriatelia) == 1) {
        if (strcmp(loginPriatelia, menoOdstranovaneho) == 0) {
            jeVPriateloch = 1;
            break;
        }
    }

    fclose(suborPriatelia);

    if (jeVPriateloch) {
        odstranZPolaPriatelov(menoOdstranovaneho);
        suborPriatelia = fopen(nazovSuboru, "a+");
        novySubor = fopen("novy_subor.txt", "a+");
        char logPriatelia[LOGIN_MAX_DLZKA];
        while (fscanf(suborPriatelia, " %s", logPriatelia) == 1) {
            if (strcmp(logPriatelia, menoOdstranovaneho) != 0) {
                fprintf(novySubor, logPriatelia);
                fprintf(novySubor, "\n");
            }
        }
        fclose(suborPriatelia);
        fclose(novySubor);
        if (remove(nazovSuboru) != 0) {
            printf("Nepodarilo sa odstranit subor\n");
        }
        if (rename("novy_subor.txt", nazovSuboru) != 0) {
            printf("Nepodarilo sa premenovat subor\n");
        }
    }
}

void vypisZoznamPriatelov() {
    printf("\n\033[35;1mKLIENT: Pocet priatelov: %d \033[0m\n", pocetPriatelov);
    printf("\033[35;1mKLIENT: Zoznam priatelov: \033[0m\n");
    int pocitadlo = 0;
    for (int i = 0; i < KLIENTI_MAX_POCET; ++i) {
        if (priatelia[i]) {
            pocitadlo++;
            printf("\t\t%d. %s\n", pocitadlo, priatelia[i]->name);
        }
    }
}

void vypisUvodneMenu() {
    puts("\033[36;1m|--- CHAT APP ---|\033[0m");
    puts("[1] Prihlasenie");
    puts("[2] Registracia");
    puts("[3] Zrusenie uctu");
    puts("[0] Koniec");
    printf("\n\033[35;1mKLIENT: Zadajte akciu: \033[0m");
}

void vypisHlavneMenu() {
    puts("\n[1] Posli spravu");
    puts("[2] Ziskaj spravu");
    puts("[3] Odhlasenie");
    puts("[4] Online uzivatelia");
    puts("[5] Pridaj priatela");
    puts("[6] Odstran priatela");
    puts("[7] Zoznam priatelov");
    puts("[0] Koniec");
    printf("\n\033[35;1mKLIENT: Zadajte akciu: \033[0m");
}

void vypisanieNovejSpravy() {
    char *odosielatel;
    char *sprava;
    odosielatel = strtok(NULL, " ");
    sprava = strtok(NULL, "\0");
    puts("Nova sprava od ineho klienta");
    printf("Nova sprava od %s: %s\n", odosielatel, sprava);
    fflush(stdout);
}

void vypisOnlineUzivatelov() {
    int pocetOnline = atoi(strtok(NULL, " "));
    if (pocetOnline <= 1) {
        printf("Okrem vas nie je online ziadny iny pouzivatel\n");
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
}

void vypisPridaniePriatelaServer() {
    char *ziadostOd;
    ziadostOd = strtok(NULL, "\0");
    pridajPriatela(ziadostOd);
    printf("\n\033[35;1mKLIENT: Pouzivatel %s si Vas pridal do priatelov!\033[0m\n", ziadostOd);
}

void vypisOdstraneniePriatelaServer() {
    char *ziadostOd;
    ziadostOd = strtok(NULL, "\0");
    odstranPriatela(ziadostOd);
    printf("\n\033[35;1mKLIENT: Pouzivatel %s si Vas odstranil z priatelov!\033[0m\n", ziadostOd);
}

void pridaniePriatelaKlient(int sockfd) {
    char menoPridavaneho[LOGIN_MAX_DLZKA];
    bzero(menoPridavaneho, LOGIN_MAX_DLZKA);
    printf("\n\033[35;1mKLIENT: Zadajte meno pouzivatela, ktoreho chcete pridat do priatelov: \033[0m");
    scanf("%s", &menoPridavaneho);
    getchar();
    if(strcmp(name, menoPridavaneho) != 0) {
        pridajPriatela(menoPridavaneho);
        posliZiadostOPriatelstvo(sockfd, menoPridavaneho);
    } else {
        printf("\n\033[35;1mKLIENT: Nemozete byt priatel sam so sebou.\033[0m\n");
    }
}

void odstraneniePriatelaKlient(int sockfd) {
    char menoOdstranovaneho[LOGIN_MAX_DLZKA];
    bzero(menoOdstranovaneho, LOGIN_MAX_DLZKA);
    printf("\n\033[35;1mKLIENT: Zadajte meno pouzivatela, ktoreho chcete odstranit z priatelov: \033[0m");
    scanf("%s", &menoOdstranovaneho);
    getchar();
    odstranPriatela(menoOdstranovaneho);
    posliInfoOOdstraneniZPriatelov(sockfd, menoOdstranovaneho);
}

int jePriatel(char *meno) {
    int ret = 0;
    for (int i = 0; i < KLIENTI_MAX_POCET; i++) {
        if (priatelia[i]) {
            if (strcmp(priatelia[i]->name, meno) == 0) {
                ret = 1;
                break;
            }
        }
    }
    return ret;
}

void spracujPrikazZoServera(char *prikaz) {


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
        puts("\n\033[35;1mKLIENT: Pridanie priatela bolo neuspesne!\033[0m\n");

    } else if (strcmp(prikaz, NEZRUSENIE_PRIATELSTVA) == 0) {
        char *meno = strtok(NULL, "\0");
        printf("MENO: %s\n", meno);
        pridajPriatela(meno);
        puts("\n\033[35;1mKLIENT: Zrusenie priatelstva bolo neuspesne!\033[0m\n");
    } else {
        printf("Spadlo to do else s takymto prikazom %s\n", prikaz);
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
            printf("Nespravne cislo akcie!\n");
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
                    printf("\n\033[35;1mKLIENT: Zadany pouzivatel nie je vo Vasom zozname priatelov.\033[0m\n");
                    return BREAK;
                }

            } else {
                prebiehaChat = chatovanie(menoPrijemcuSpravy, sockfd);
            }
            if (prebiehaChat == 0) {
                zacalChat = 0;
                return BREAK;
            }
        } else if (akcia == 2) {
            return CONTINUE;

        } else if (akcia == 3) {
            jePrihlaseny = 0;
            return BREAK;

        } else if (akcia == 4) {
            onlineUzivatelia(sockfd);
        } else if (akcia == 5) {
            pridaniePriatelaKlient(sockfd);
            return BREAK;

        } else if (akcia == 6) {
            odstraneniePriatelaKlient(sockfd);
            return BREAK;

        } else if (akcia == 7) {
            vypisZoznamPriatelov();
            return BREAK;
        } else {
            printf("Nespravne cislo akcie!\n");
            return BREAK;
        }
    }
    return "ok";
}


