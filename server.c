//
// Created by Matúš Baláži on 28. 12. 2021.
//

#include "server.h"

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <langinfo.h>


int main(int argc, char *argv[]) {
    int sockfd, newsockfd;
    socklen_t cli_len;
    struct sockaddr_in serv_addr, cli_addr;
    int n;
    char buffer[256];

    if (argc < 2) {
        fprintf(stderr, "usage %s port\n", argv[0]);
        return 1;
    }

    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(atoi(argv[1]));

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        perror("Error creating socket");
        return 1;
    }

    if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
        perror("Error binding socket address");
        return 2;
    }

    printf("\033[32;1mSERVER: Zapnutie prebehlo uspesne.\033[0m\n");
    listen(sockfd, 5);

    //nadviazeme spojenie s jednym klientskym socketom
    cli_len = sizeof(cli_addr);
    newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &cli_len);
    if (newsockfd < 0) {
        perror("ERROR on accept");
        return 3;
    }

    int bolExit = 0;
    while (bolExit == 0) {
        bzero(buffer, 256);
        n = read(newsockfd, buffer, 255);
        //citanie buffra co prisiel na socket
        if (n < 0) {
            perror("Error reading from socket");
            return 4;
        }
        char *typSpravy;
        typSpravy = strtok(buffer, " ");

        if (strcmp(typSpravy, "registracia") == 0) {
            int res = spracovanieRegistracie(newsockfd, n);

        } else if (strcmp(typSpravy, "prihlasenie") == 0) {
            int res = spracovaniePrihlasenia(newsockfd, n);

        } else if (strcmp(typSpravy, "chatovanie") == 0) {
            int res = spracovanieChatovania(newsockfd, n);

        } else if (strcmp(typSpravy, "exit") == 0) {
            bolExit = 1;

        }
    }
    close(newsockfd);
    close(sockfd);

    return 0;
}

int spracovaniePrihlasenia(int newsockfd, int n) {
    char *login;
    char *heslo;

    login = strtok(NULL, " ");
    heslo = strtok(NULL, "/0");

    printf("LOGIN: %s\n", login);
    printf("HESLO: %s\n", heslo);

    int log = prihlasenie(login, heslo);
    printf("VYSLEDOK PRIHLASENIA: %d\n", log);

    if (log == 1) {
        const char *msg = "\n\033[32;1mSERVER: Uspesne prihlasenie.\033[0m\n";
        n = write(newsockfd, msg, strlen(msg) + 1);
        if (n < 0) {
            perror("Error writing to socket");
            return 5;
        }
    } else {
        const char *msg = "\n\033[32;1mSERVER: Neuspesne prihlasenie. Nespravne meno alebo heslo.\033[0m\n";
        n = write(newsockfd, msg, strlen(msg) + 1);
        if (n < 0) {
            perror("Error writing to socket");
            return 5;
        }
    }
}

int spracovanieRegistracie(int newsockfd, int n) {
    char *login;
    char *heslo;
    char *potvrdeneHeslo;

    login = strtok(NULL, " ");
    heslo = strtok(NULL, " ");
    potvrdeneHeslo = strtok(NULL, "/0");

    printf("LOGIN: %s\n", login);
    printf("HESLO: %s\n", heslo);
    printf("POTVRDENE HESLO: %s\n", potvrdeneHeslo);

    int reg = registracia(login, heslo, potvrdeneHeslo);

    printf("VYSLEDOK REGISTRACIE: %d\n", reg);

    if (reg == 1) {
        const char *msg = "\n\033[32;1mSERVER: Uspesna registracia.\033[0m\n";
        n = write(newsockfd, msg, strlen(msg) + 1);
        if (n < 0) {
            perror("Error writing to socket");
            return 5;
        }
    } else if (reg == 0) {
        const char *msg = "\n\033[32;1mSERVER: Neuspesna registracia. Zadany login uz existuje.\033[0m\n";
        n = write(newsockfd, msg, strlen(msg) + 1);
        if (n < 0) {
            perror("Error writing to socket");
            return 5;
        }
    } else {
        const char *msg = "\n\033[32;1mSERVER: Neuspesna registracia. Hesla sa nezhoduju.\033[0m\n";
        n = write(newsockfd, msg, strlen(msg) + 1);
        if (n < 0) {
            perror("Error writing to socket");
            return 5;
        }
    }
    return 0;
}

int spracovanieChatovania(int newsockfd, int n) {
    char *sprava;
    sprava = strtok(NULL, "/0");
    printf("SPRAVA: %s\n", sprava);

    //toto nechce vypisat z nejakeho dovodu, treba sa na to pozriet
    printf("\n\033[32;1mSERVER: Bola prijata sprava:\033[0m %s\n", sprava);

    const char *msg = "\n\033[32;1mSERVER: Sprava bola obdrzana.\033[0m\n";
    n = write(newsockfd, msg, strlen(msg) + 1);
    if (n < 0) {
        perror("Error writing to socket");
        return 5;
    }
}



