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
    FILE* subor = fopen("zaregistrovani_pouzivatelia.txt", "a+");
    bool jeUzZaregistrovany = false;
    while (subor != EOF) {
        if (strcmp(fscanf(subor, "%s"), login) == 0) {
            jeUzZaregistrovany = true;
            break;
        }
    }

    if (!jeUzZaregistrovany) {
        fprintf(subor, login);
        fprintf(subor, " ");
        fprintf(subor, potvrdHeslo);
        fprintf(subor, "\n");
        return 1;
    } else {
        // Zadany login sa v databaze pouzivatelov uz nachadza
        return 0;
    }
}