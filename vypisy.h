//
// Created by Danko on 04/01/2022.
//

#ifndef POS_ZAPOCET_2_VYPISY2_H
#define POS_ZAPOCET_2_VYPISY2_H

#endif //POS_ZAPOCET_2_VYPISY_H

#include <stdlib.h>
#include "priatelia.h"

void vypisUvodneMenu() {
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

void vypisNovejSpravy(char *typSpravy) {
    char *odosielatel;
    char *sprava;
    odosielatel = strtok(NULL, " ");
    sprava = strtok(NULL, "\0");
    if ((strcmp(typSpravy, SPRAVA_PRIJIMATELOVI) == 0)) {
        printf("\n\033[34;1mNOVA\033[0m \033[33;1mSUKROMNA\033[0m \033[34;1mSPRAVA od \033[0m%s\033[34;1m:\033[0m %s",
               odosielatel, sprava);
    } else {
        printf("\n\033[34;1mNOVA\033[0m \033[32;1mSKUPINOVA\033[0m \033[34;1mSPRAVA od \033[0m%s\033[34;1m:\033[0m %s",
               odosielatel, sprava);
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
        if (strcmp(meno, clientName) == 0) {
            printf("\t\t\033[36;1mVy:\033[0m %s\n", meno);
        } else {
            pocitadlo++;
            printf("\t\t\033[34;1m%d.\033[0m %s \n", pocitadlo, meno);
        }
    }
    printf("\n");
}

void vypisPridaniePriatela() {
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

void vypisNemoznoSpriatelit() {
    char *meno = strtok(NULL, "\0");
    printf("MENO: %s\n", meno);
    odstranPriatela(meno);
    puts("\n\033[31;1mKLIENT: Pridanie priatela bolo neuspesne!\033[0m\n");
}

void vypisNezruseniePriatelstva() {
    char *meno = strtok(NULL, "\0");
    printf("MENO: %s\n", meno);
    pridajPriatela(meno);
    puts("\n\033[31;1mKLIENT: Zrusenie priatelstva bolo neuspesne!\033[0m\n");
}

void vypisSpravaPrijataServerom() {
    puts("\n\033[32;1mKLIENT: Sprava bola prijata serverom na spracovanie!\033[0m\n");
}

void vypisNeuspesnaRegistracia() {
    puts("\n\033[31;1mKLIENT: Registracia bola neuspesna!\033[0m\n");
}

void vypisUspesnaRegistracia() {
    puts("\n\033[32;1mKLIENT: Registracia prebehla uspesne!\033[0m\n");
}

void vypisNeuspesnePrihlasenie() {
    puts("\n\033[31;1mKLIENT: Prihlasenie bolo neuspesne!\033[0m\n");
}

void vypisUspesnePrihlasenie() {
    puts("\n\033[32;1mKLIENT: Prihlasenie prebehlo uspesne!\033[0m\n");
}

void vypisNeuspesneZrusenie() {
    puts("\n\033[31;1mKLIENT: Zrusenie uctu bolo neuspesne!\033[0m\n");
}

void vypisUspesneZrusenieUctu() {
    puts("\n\033[32;1mKLIENT: Zrusenie uctu prebehlo uspesne!\033[0m\n");
}

void vypisNeznamyPrikaz(const char *prikaz) {
    printf("\n\033[31;1mKlient prijal neznamy prikaz od serveru:\033[0m %s\n", prikaz);
}

void vypisUkoncenieChatu(char *name) {
    printf("\n\033[32;1mSERVER: Pouzivatel\033[0m %s \033[32;1mukoncil chat.\033[0m\n", name);
}