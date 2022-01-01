//
// Created by Matúš Baláži on 28. 12. 2021.
//

#ifndef POS_ZAPOCET_2_KLIENT_H
#define POS_ZAPOCET_2_KLIENT_H

#endif //POS_ZAPOCET_2_KLIENT_H

int registracia(char buffer[], int sockfd, int n);

int prihlasenie(char buffer[], int sockfd, int n);

int chatovanie(char buffer[], int sockfd);

int zrusenieUctu(char buffer[], int sockfd, int n);

int uvodnaObrazovka(char buffer[], int sockfd, int n);

int hlavnaPonuka(char buffer[], int sockfd, int n);

void writeToServer(char buffer[], int sockfd);

void listenToServer(char buffer[], int sockfd);

void klientovCyklus(int sockfd);