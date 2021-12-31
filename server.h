//
// Created by Matúš Baláži on 28. 12. 2021.
//
#include <stdio.h>
#include <stdbool.h>
#include <string.h>

#ifndef POS_ZAPOCET_2_SERVER_H
#define POS_ZAPOCET_2_SERVER_H

#endif //POS_ZAPOCET_2_SERVER_H

int registracia(char *login, char *heslo, char *potvrdHeslo) {

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
            printf("\n\033[32;1mSERVER: Vykonala sa registracia noveho pouzivatela %s.\033[0m\n", login);
            return 1;
        } else {
            // Zadane heslo a potvrdene heslo sa nezhoduju
            fclose(subor);
            printf("\n\033[32;1mSERVER: Hesla sa nezhoduju.\033[0m\n");
            return 2;
        }
    } else {
        // Zadany login sa v databaze pouzivatelov uz nachadza
        fclose(subor);
        printf("\n\033[32;1mSERVER: Login %s uz existuje.\033[0m\n", pomocna);
        return 0;
    }
}

int prihlasenie(char *login, char *heslo) {
    FILE *subor;
    subor = fopen("zaregistrovani_pouzivatelia.txt", "r");
    int foundLogin = 0;
    char loginFile[30];
    char hesloFile[30];
    while (fscanf(subor, " %s %s", loginFile, hesloFile) == 2) {
        if ((strcmp(loginFile, login) == 0) && strcmp(hesloFile, heslo) == 0) {
            foundLogin = 1;
            break;
        }
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

int zrusenieUctu(char* login, char* heslo) {
    FILE *subor, *novySubor;
    subor = fopen("zaregistrovani_pouzivatelia.txt", "a+");

    int foundLogin = 0;
    char loginFile[30];
    char hesloFile[30];
    while (fscanf(subor, " %s %s", loginFile, hesloFile) == 2) {
        if ((strcmp(loginFile, login) == 0) && strcmp(hesloFile, heslo) == 0) {
            foundLogin = 1;
            break;
        }
    }

    fclose(subor);

    if (foundLogin == 1) {
        subor = fopen("zaregistrovani_pouzivatelia.txt", "a+");
        novySubor = fopen("novy_subor.txt", "a+");
        printf("\n\033[32;1mSERVER: Pouzivatel %s si zrusil ucet\033[0m\n", loginFile);
        char log[30];
        char pass[30];
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
        if (remove("zaregistrovani_pouzivatelia.txt") != 0) {
            printf("Nepodarilo sa odstranit subor\n");
        }
        if (rename("novy_subor.txt", "zaregistrovani_pouzivatelia.txt") != 0) {
            printf("Nepodarilo sa premenovat subor\n");
        }
        return 1;
    } else {
        printf("\n\033[32;1mSERVER: Nespravne meno alebo heslo\033[0m\n");
        return 0;
    }
}

void spracovanieRegistracie(int newsockfd, int n);

void spracovaniePrihlasenia(int newsockfd, int n);

void spracovanieChatovania(int newsockfd, int n);

void spracovanieZruseniaUctu(int newsockfd, int n);

void writeToClient(char buffer[], int sockfd);

void listenToClient(char buffer[], int sockfd);