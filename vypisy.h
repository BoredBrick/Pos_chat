//
// Created by Danko on 04/01/2022.
//

#ifndef POS_ZAPOCET_2_VYPISY2_H
#define POS_ZAPOCET_2_VYPISY2_H

#endif //POS_ZAPOCET_2_VYPISY_H

#include "priatelia.h"

void vypisUvodneMenu(){
    puts("\033[36;1m|--- CHAT APP ---|\033[0m");
    puts("[1] Prihlasenie");
    puts("[2] Registracia");
    puts("[3] Zrusenie uctu");
    puts("[0] Koniec");
    printf("\n\033[35;1mKLIENT: Zadajte akciu: \033[0m");
}

void vypisHlavneMenu() {
    puts("\n\033[36;1m|--- CHAT APP ---|\033[0m");
    puts("[1] Sukromny chat");
    puts("[2] Skupinovy chat");
    puts("[3] Online uzivatelia");
    puts("[4] Pridaj priatela");
    puts("[5] Odstran priatela");
    puts("[6] Zoznam priatelov");
    puts("[7] Odhlasenie");
    puts("[0] Koniec");
    printf("\n\033[35;1mKLIENT: Zadajte akciu: \033[0m");
}

void vypisanieSpravyZHistorie(char *odosielatel, char *prijimatel, char *sprava) {
    printf("\n\033[90;1m%s>%s: %s\033[0m", odosielatel, prijimatel, sprava);
}

void vypisanieNovejSpravy(char* typSpravy) {
    char *odosielatel;
    char *sprava;
    odosielatel = strtok(NULL, " ");
    sprava = strtok(NULL, "\0");
    if((strcmp(typSpravy, SPRAVA_PRIJIMATELOVI) == 0)) {
        printf("\n\033[34;1mNOVA\033[0m \033[33;1mSUKROMNA\033[0m \033[34;1mSPRAVA od \033[0m%s\033[34;1m:\033[0m %s\n", odosielatel, sprava);
    } else {
        printf("\n\033[34;1mNOVA\033[0m \033[32;1mSKUPINOVA\033[0m \033[34;1mSPRAVA od \033[0m%s\033[34;1m:\033[0m %s\n", odosielatel, sprava);
    }
    fflush(stdout);
}

void vypisOnlineUzivatelov() {
    int pocetOnline = atoi(strtok(NULL, " "));
    if (pocetOnline <= 1) {
        printf("\n\033[33;1mOkrem vas nie je online ziadny iny pouzivatel.\033[0m\n");
    } else {
        printf("\n\033[33;1mNasledujuci pouzivatelia su online: \033[0m\n");
    }
    int pocitadlo = 0;
    for (int i = 0; i < pocetOnline; ++i) {
        char *meno = strtok(NULL, " ");
        if (strcmp(meno, name) == 0) {
            printf("\t\t\033[36;1mVy:\033[0m %s\n", meno);
        } else {
            pocitadlo++;
            printf("\t\t\033[34;1m%d.\033[0m %s \n", pocitadlo, meno);
        }
    }
    printf("\n");
}

void vypisPridaniePriatelaServer() {
    char *ziadostOd;
    ziadostOd = strtok(NULL, "\0");
    pridajPriatela(ziadostOd);
    printf("\n\033[34;1mKLIENT: Pouzivatel\033[0m %s \033[34;1msi Vas pridal do priatelov!\033[0m\n", ziadostOd);
}

void vypisOdstraneniePriatelaServer() {
    char *ziadostOd;
    ziadostOd = strtok(NULL, "\0");
    odstranPriatela(ziadostOd);
    printf("\n\033[34;1mKLIENT: Pouzivatel\033[0m %s \033[34;1msi Vas odstranil z priatelov!\033[0m\n", ziadostOd);
}
