//
// Created by Danko on 04/01/2022.
//

#ifndef POS_CHAT_PRIATELIA2_H
#define POS_CHAT_PRIATELIA2_H

#endif //POS_CHAT_PRIATELIA2_H
#include "klient_server_komunikacia.h"
#include "procedury.h"

typedef struct {
    char name[LOGIN_MAX_DLZKA];
} priatel;
priatel *priatelia[MAX_POCET_PRIATELOV];
char name[LOGIN_MAX_DLZKA];
int pocetPriatelov = 0;

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
    printf("\n\033[33;1mKLIENT: Pocet priatelov:\033[0m %d\n", pocetPriatelov);
    printf("\033[33;1mKLIENT: Zoznam priatelov: \033[0m\n");
    int pocitadlo = 0;
    for (int i = 0; i < KLIENTI_MAX_POCET; ++i) {
        if (priatelia[i]) {
            pocitadlo++;
            printf("\t\t\033[34;1m%d.\033[0m %s\n", pocitadlo, priatelia[i]->name);
        }
    }
    printf("\n");
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
        printf("\n\033[31;1mKLIENT: Nemozete byt priatel sam so sebou.\033[0m\n");
    }
}

void odstraneniePriatelaKlient(int sockfd) {
    char menoOdstranovaneho[LOGIN_MAX_DLZKA];
    bzero(menoOdstranovaneho, LOGIN_MAX_DLZKA);
    printf("\n\033[35;1mKLIENT: Zadajte meno pouzivatela, ktoreho chcete odstranit z priatelov: \033[0m");
    scanf("%s", &menoOdstranovaneho);
    getchar();
    if(jePriatel(menoOdstranovaneho)) {
        odstranPriatela(menoOdstranovaneho);
        posliInfoOOdstraneniZPriatelov(sockfd, menoOdstranovaneho);
    } else {
        printf("\n\033[31;1mKLIENT: Odstranenie priatela bolo neuspesne.\033[0m\n");
    }
}


