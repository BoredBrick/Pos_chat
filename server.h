//
// Created by Matúš Baláži on 28. 12. 2021.
//
#include <stdio.h>
#include <stdbool.h>
#include <string.h>

#ifndef POS_ZAPOCET_2_SERVER_H
#define POS_ZAPOCET_2_SERVER_H

#endif //POS_ZAPOCET_2_SERVER_H

int registracia(char* login, char* heslo, char* potvrdHeslo) {

    FILE *subor;
    subor = fopen("zaregistrovani_pouzivatelia.txt", "a+");
    char pomocna[512];

    int jeUzZaregistrovany = 0;
    while (fscanf(subor, " %s", pomocna) == 1) {
        if (strcmp(pomocna, login) == 0) {
            jeUzZaregistrovany = 1;
            break;
        }
    }

    if (jeUzZaregistrovany == 0) {
        if (strcmp(heslo, potvrdHeslo) == 0) {
            fprintf(subor, login);
            fprintf(subor, " ");
            fprintf(subor, heslo);
            fprintf(subor, "\n");
            fclose(subor);
            printf("\n\033[32;1mSERVER: Vykonala sa registracia noveho pouzivatela.\033[0m\n");
            return 1;
        } else {
            // Zadane heslo a potvrdene heslo sa nezhoduju
            printf("\n\033[32;1mSERVER: Hesla sa nezhoduju.\033[0m\n");
            return 2;
        }
    } else {
        // Zadany login sa v databaze pouzivatelov uz nachadza
        printf("\n\033[32;1mSERVER: Login %s uz existuje.\033[0m\n", pomocna);
        return 0;
    }
}

int prihlasenie(char* login, char* heslo) {
    FILE *subor;
    subor = fopen("zaregistrovani_pouzivatelia.txt", "r");
    int foundLogin = 0;
    char buff[512];
    char loginFile[30];
    char hesloFile[30];
    while (fscanf(subor, " %s %s", loginFile, hesloFile) == 2) {
        if ((strcmp(loginFile, login) == 0) && strcmp(hesloFile, heslo) == 0) {
            foundLogin = 1;
            break;
        }
    }

    if(foundLogin == 1) {
        printf("\n\033[32;1mSERVER: Uzivatel  %s sa prihlasil\033[0m\n", loginFile);
        return 1;
    } else {
        printf("\n\033[32;1mSERVER: Nespravne meno alebo heslo\033[0m\n");
        return 0;
    }

}


int spracovanieRegistracie(int newsockfd, int n);

int spracovaniePrihlasenia(int newsockfd, int n);

int spracovanieChatovania(int newsockfd, int n);