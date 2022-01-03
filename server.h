//
// Created by Matúš Baláži on 28. 12. 2021.
//
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <malloc.h>
#include "procedury.h"

#ifndef POS_ZAPOCET_2_SERVER_H
#define POS_ZAPOCET_2_SERVER_H

#endif //POS_ZAPOCET_2_SERVER_H


int registracia(char *login, char *heslo, char *potvrdHeslo) {

    FILE *subor;
    subor = fopen(MENO_SUBORU, "a+");
    char suborLogin[BUFFER_SIZE];
    bzero(suborLogin, BUFFER_SIZE);
    char suborHeslo[BUFFER_SIZE];
    bzero(suborHeslo, BUFFER_SIZE);

    int jeUzZaregistrovany = 0;

    while (fscanf(subor, "%s %s", suborLogin, suborHeslo) == 2) {
        if (strcmp(suborLogin, login) == 0) {
            jeUzZaregistrovany = 1;
            break;
        }
        bzero(suborLogin, BUFFER_SIZE);
        bzero(suborHeslo, BUFFER_SIZE);

    }

    if (jeUzZaregistrovany == 0) {
        int pocetZnakovLogin = 0;
        int pocetZnakovHeslo = 0;
        for (int i = 0; i < strlen(login); ++i) {
            pocetZnakovLogin++;
        }
        for (int i = 0; i < strlen(heslo); ++i) {
            pocetZnakovHeslo++;
        }
        if (pocetZnakovLogin >= 4 && pocetZnakovHeslo >= 4) {
            if (strcmp(heslo, potvrdHeslo) == 0) {
                char *pomPassword = malloc(sizeof(char) * strlen(heslo));
                sifrujRetazec(pomPassword, heslo);
                fprintf(subor, login);
                fprintf(subor, " ");
                fprintf(subor, pomPassword);
                fprintf(subor, "\n");
                fclose(subor);
                printf("\n\033[32;1mSERVER: Vykonala sa registracia noveho pouzivatela %s.\033[0m\n", login);
                free(pomPassword);
                return 1;
            } else {
                // Zadane heslo a potvrdene heslo sa nezhoduju
                fclose(subor);
                printf("\n\033[32;1mSERVER: Hesla sa nezhoduju.\033[0m\n");
                return 2;
            }
        } else {
            // Zadany login alebo heslo nesplnuju poziadavky
            fclose(subor);
            printf("\n\033[32;1mSERVER: Login alebo heslo su prilis kratke.\033[0m\n");
            return 2;
        }

    } else {
        // Zadany login sa v databaze pouzivatelov uz nachadza
        fclose(subor);
        printf("\n\033[32;1mSERVER: Login %s uz existuje.\033[0m\n", suborLogin);
        return 0;
    }
}

int prihlasenie(char *login, char *heslo) {
    char pomPassword[HESLO_MAX_DLZKA];
    FILE *subor;
    subor = fopen(MENO_SUBORU, "r");
    int foundLogin = 0;
    char loginFile[LOGIN_MAX_DLZKA];
    char hesloFile[HESLO_MAX_DLZKA];
    while (fscanf(subor, " %s %s", loginFile, hesloFile) == 2) {
        odsifrujRetazec(pomPassword, hesloFile);
        if ((strcmp(loginFile, login) == 0) && strcmp(pomPassword, heslo) == 0) {
            foundLogin = 1;
            break;
        }
        bzero(pomPassword,30);

        bzero(loginFile,LOGIN_MAX_DLZKA);
        bzero(hesloFile,HESLO_MAX_DLZKA);

    }

    fclose(subor);
    if (foundLogin == 1) {
        printf("\n\033[32;1mSERVER: Pouzivatel %s sa prihlasil\033[0m\n", loginFile);
        return 1;
    } else {
        printf("\n\033[32;1mSERVER: Nespravne meno alebo heslo\033[0m\n");
        return 0;
    }

}

int zrusenieUctu(char *login, char *heslo) {
    char pomPassword[HESLO_MAX_DLZKA];
    FILE *subor, *novySubor;
    subor = fopen(MENO_SUBORU, "r");
    int foundLogin = 0;
    char loginFile[LOGIN_MAX_DLZKA];
    char hesloFile[HESLO_MAX_DLZKA];
    while (fscanf(subor, " %s %s", loginFile, hesloFile) == 2) {
        odsifrujRetazec(pomPassword, hesloFile);
        if ((strcmp(loginFile, login) == 0) && strcmp(pomPassword, heslo) == 0) {
            foundLogin = 1;
            break;
        }
        bzero(pomPassword,30);

        bzero(loginFile,LOGIN_MAX_DLZKA);
        bzero(hesloFile,HESLO_MAX_DLZKA);

    }

    fclose(subor);

    if (foundLogin == 1) {
        subor = fopen(MENO_SUBORU, "a+");
        novySubor = fopen("novy_subor.txt", "a+");
        printf("\n\033[32;1mSERVER: Pouzivatel %s si zrusil ucet\033[0m\n", loginFile);
        char log[LOGIN_MAX_DLZKA];
        char pass[HESLO_MAX_DLZKA];
        while (fscanf(subor, " %s %s", log, pass) == 2) {
            if (strcmp(login, log) != 0) {
                fprintf(novySubor, log);
                fprintf(novySubor, " ");
                fprintf(novySubor, pass);
                fprintf(novySubor, "\n");
            }
        }
        fclose(subor);
        fclose(novySubor);
        if (remove(MENO_SUBORU) != 0) {
            printf("Nepodarilo sa odstranit subor\n");
        }
        if (rename("novy_subor.txt", MENO_SUBORU) != 0) {
            printf("Nepodarilo sa premenovat subor\n");
        }

        char nazovSuboru[BUFFER_SIZE];
        strcat(nazovSuboru, login);
        strcat(nazovSuboru, ".txt");

        // vymazanie obsahu suboru
        FILE *priatelia;
        priatelia = fopen(nazovSuboru, "w");
        fclose(priatelia);

        if (remove(nazovSuboru) != 0) {
            printf("Nepodarilo sa odstranit subor\n");
        }
        return 1;
    } else {
        printf("\n\033[32;1mSERVER: Nespravne meno alebo heslo\033[0m\n");
        return 0;
    }
}

int najdiSocketPodlaMena(char *meno);

void oznamenieOPriatelstve();

void oznamenieOOdstraneniZPriatelov();

void zoznamOnlinePouzivatelov(int newsockfd);

void spracovanieRegistracie(int newsockfd);

void spracovaniePrihlasenia(int newsockfd);

void spracovanieChatovania(int clientSockFD);

void spracovanieZruseniaUctu(int clientSockFD);

void writeToClient(char buffer[], int sockfd);

void listenToClient(char buffer[], int sockfd);