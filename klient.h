//
// Created by Matúš Baláži on 28. 12. 2021.
//

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "procedury.h"

#ifndef POS_ZAPOCET_2_KLIENT_H
#define POS_ZAPOCET_2_KLIENT_H

#endif //POS_ZAPOCET_2_KLIENT_H

char name[LOGIN_MAX_DLZKA];
int jePrihlaseny = 0;

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

int registracia(char buffer[], int sockfd) {
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

int prihlasenie(char buffer[], int sockfd) {
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

int chatovanie(char buffer[], int sockfd) {
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

int zrusenieUctu(char buffer[], int sockfd) {
    char login[LOGIN_MAX_DLZKA], heslo[HESLO_MAX_DLZKA];
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

int ukoncenieAplikacie(char buffer[], int sockfd) {
    bzero(buffer, BUFFER_SIZE);
    strcat(buffer, "exit");
    sifrujRetazec(buffer, buffer);
    writeToServer(buffer, sockfd);
}

void klientovCyklus(int sockfd);

void klientovCyklus2(int sockfd);

void onlineUzivatelia(int sockfd) {
    char buffer[BUFFER_SIZE];
    bzero(buffer,BUFFER_SIZE);
    strcat(buffer, ONLINE_UZIVATELIA);
    sifrujRetazec(buffer, buffer);

    // Poslanie udajov serveru
    writeToServer(buffer, sockfd);
}


