//
// Created by Matúš Baláži on 2. 1. 2022.
//

#ifndef POS_znakAT_PROCEDURY_H
#define POS_znakAT_PROCEDURY_H

#endif //POS_znakAT_PROCEDURY_H

// SPRAVY OD KLIENTA
const char* PRIHLASENIE = "prihlasenie";
const char* REGISTRACIA = "registracia";
const char* ZRUSENIE_UCTU = "zrusenie_uctu";
const char* CHATOVANIE = "chatovanie";
const char* UKONCENIE_CHATOVANIA = "ukoncenie_chatovania";
const char* EXIT = "exit";

// SPRAVY OD SERVERA
const char* USPESNA_REGISTRACIA = "uspesna_registracia";
const char* NEUSPESNA_REGISTRACIA = "neuspesna_registracia";
const char* USPESNE_PRIHLASENIE = "uspesne_prihlasenie";
const char* NEUSPESNE_PRIHLASENIE = "neuspesne_prihlasenie";
const char* USPESNE_ZRUSENIE = "uspesne_zrusenie";
const char* NEUSPESNE_ZRUSENIE = "neuspesne_zrusenie";
const char* SPRAVA_ODOSIELATELOVI = "sprava_odosielatelovi";
const char* SPRAVA_PRIJIMATELOVI = "sprava_prijimatelovi";

// INE
#define KLUC 5
#define BUFFER_SIZE 256
#define MESSAGE_MAX_DLZKA 30
#define LOGIN_MAX_DLZKA 30
#define HESLO_MAX_DLZKA 60
#define SPRAVA_MAX_DLZKA 100
#define KLIENTI_MAX_POCET 10
const char* MENO_SUBORU = "zaregistrovani_pouzivatelia.txt";

void sifrujRetazec(char *vysledok, char *retazec) {
    for (int i = 0; retazec[i] != '\0'; ++i) {
        char znak = retazec[i];
        if (znak >= 'a' && znak <= 'z') {
            znak = znak + KLUC;
            if (znak > 'z') {
                znak = znak - 'z' + 'a' - 1;
            }
            vysledok[i] = znak;
        } else if (znak >= 'A' && znak <= 'Z') {
            znak = znak + KLUC;
            if (znak > 'Z') {
                znak = znak - 'Z' + 'A' - 1;
            }
            vysledok[i] = znak;
        } else if (znak >= '0' && znak <= '9') {
            znak = znak + KLUC;
            if (znak > '9') {
                znak = znak - '9' + '0' - 1;
            }
            vysledok[i] = znak;
        } else {
            vysledok[i] = retazec[i];
        }
    }
}

void odsifrujRetazec(char *vysledok, char *retazec) {
    for (int i = 0; retazec[i] != '\0'; ++i) {
        char znak = retazec[i];
        if (znak >= 'a' && znak <= 'z') {
            znak = znak - KLUC;
            if (znak < 'a') {
                znak = znak + 'z' - 'a' + 1;
            }
            vysledok[i] = znak;
        } else if (znak >= 'A' && znak <= 'Z') {
            znak = znak - KLUC;
            if (znak < 'A') {
                znak = znak + 'Z' - 'A' + 1;
            }
            vysledok[i] = znak;
        } else if (znak >= '0' && znak <= '9') {
            znak = znak - KLUC;
            if (znak < '0') {
                znak = znak + '9' - '0' + 1;
            }
            vysledok[i] = znak;
        } else {
            vysledok[i] = retazec[i];
        }
    }
}