//
// Created by Matúš Baláži on 28. 12. 2021.
//

#include "server.h"
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

static _Atomic unsigned int cli_count = 0;
static int clientIDstart = 10;

// CLIENT STRUCTURE
typedef struct {
    struct sockaddr_in address;
    int sockfd;
    int clientID;
    char name[LOGIN_MAX_DLZKA];
} client;
client *klienti[KLIENTI_MAX_POCET];

pthread_mutex_t clients_mutex = PTHREAD_MUTEX_INITIALIZER;

void pridajKlienta(client *client) {
    pthread_mutex_lock(&clients_mutex);

    for (int i = 0; i < KLIENTI_MAX_POCET; i++) {
        if (!klienti[i]) {
            klienti[i] = client;
            break;
        }
    }

    pthread_mutex_unlock(&clients_mutex);
}

void odoberKlienta(int clientID) {
    pthread_mutex_lock(&clients_mutex);

    for (int i = 0; i < KLIENTI_MAX_POCET; i++) {
        if (klienti[i]) {
            if (klienti[i]->clientID == clientID) {
                klienti[i] = NULL;
                break;
            }
        }
    }
    pthread_mutex_unlock(&clients_mutex);
}

//posle spravu konkretnemu clientID
void send_message(char *s, char *komu) {
//    pthread_mutex_lock(&clients_mutex);
//    int socket = najdiSocketPodlaMena(komu);
//    writeToClient(s,socket);
//    pthread_mutex_unlock(&clients_mutex);
    pthread_mutex_lock(&clients_mutex);
    for (int i = 0; i < KLIENTI_MAX_POCET; i++) {
        if (klienti[i]) {
            if (strcmp(klienti[i]->name, komu) == 0) {
                printf("Zapisujem na socket: %d\n", klienti[i]->sockfd);
                sifrujRetazec(s, s);
                if (write(klienti[i]->sockfd, s, strlen(s)) < 0) {
                    printf("ERROR: write to descriptor failed\n");
                }
                break;
            }
        }
    }
    pthread_mutex_unlock(&clients_mutex);
}

//funkcia, ktora bude vo vlakne cakat na to, pokial nedostane spravu od klienta, potom ju spracuje
void *obsluhaKlienta(void *arg) {
    printf("\033[32;1mSERVER: Bolo vytvorene vlakno pre klienta!\033[0m\n");
    char buffer[BUFFER_SIZE];
    bzero(buffer, BUFFER_SIZE);
    cli_count++;

    client *cli = (client *) arg;

    printf("\033[32;1mSERVER: Pripojil sa klient so socketom: %d\033[0m\n", cli->sockfd);

    int bolExit = 0;
    int clientSockFD = cli->sockfd;

    while (bolExit == 0) {
        listenToClient(buffer, clientSockFD);
        odsifrujRetazec(buffer, buffer);

        char *typSpravy;
        typSpravy = strtok(buffer, " ");

        if (strcmp(typSpravy, REGISTRACIA) == 0) {
            spracovanieRegistracie(clientSockFD);

        } else if (strcmp(typSpravy, PRIHLASENIE) == 0) {
            spracovaniePrihlasenia(clientSockFD);

        } else if (strcmp(typSpravy, CHATOVANIE) == 0) {
            spracovanieChatovania(clientSockFD);

        } else if (strcmp(typSpravy, ZRUSENIE_UCTU) == 0) {
            spracovanieZruseniaUctu(clientSockFD);

        } else if (strcmp(typSpravy, EXIT) == 0) {
            bolExit = 1;

        } else if (strcmp(typSpravy, UKONCENIE_CHATOVANIA) == 0) {
            printf("Ukoncenie chatovania\n");

        } else if (strcmp(typSpravy, ONLINE_UZIVATELIA) == 0) {
            zoznamOnlinePouzivatelov(clientSockFD);
        } else if (strcmp(typSpravy, NOVY_PRIATEL) == 0) {
            oznamenieOPriatelstve(clientSockFD);

        } else if (strcmp(typSpravy, ZRUS_PRIATELA) == 0) {
            oznamenieOOdstraneniZPriatelov(clientSockFD);

        }
    }

    close(clientSockFD);
    odoberKlienta(cli->clientID);
    free(cli);
    cli_count--;
    pthread_detach(pthread_self());
    return NULL;
}

int main(int argc, char *argv[]) {
    int sockfd, clientSockFD;
    socklen_t cli_len;
    // server address, client address
    struct sockaddr_in serv_addr, cli_addr;

    if (argc < 2) {
        fprintf(stderr, "usage %s port\n", argv[0]);
        return 1;
    }

    // SOCKET SETTINGS
    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(atoi(argv[1]));


    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        perror("Error creating socket");
        return 1;
    }

    // BINDING
    if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
        perror("Error binding socket address");
        return 2;
    }


    // Tento vypis dat skorej asi pod LISTENING
    printf("\033[32;1mSERVER: Zapnutie prebehlo uspesne.\033[0m\n");

    //LISTENING
    listen(sockfd, 5);


    while (1) {
        cli_len = sizeof(cli_addr);
        // CONNECTION TO CLIENT
        clientSockFD = accept(sockfd, (struct sockaddr *) &cli_addr, &cli_len);
        if (clientSockFD < 0) {
            perror("ERROR on accept");
            return 3;
        }
        /*      // CHECK FOR MAX_CLIENTS
         *      if ((cli_count + 1) == MAX_CLIENTS) {
         *          printf("Maximum klienti connected. Connection rejected.\n");
         *          print_ip_addr(cli_addr);
         *          close(clientSockFD);
         *          continue;
         *      }
         */
        // CLIENT SETTINGS
        client *cli = (client *) malloc(sizeof(client));
        cli->address = cli_addr;
        cli->sockfd = clientSockFD;
        cli->clientID = clientIDstart++;

        pridajKlienta(cli);

        pthread_t vlakno;
        pthread_create(&vlakno, NULL, &obsluhaKlienta, (void *) cli);

        // REDUCE CPU USAGE
        sleep(1);

    }

    close(sockfd);

    return 0;
}

void spracovaniePrihlasenia(int clientSockFD) {
    char *login;
    char *heslo;

    login = strtok(NULL, " ");
    heslo = strtok(NULL, "/0");

    int log = prihlasenie(login, heslo);

    char msg[MESSAGE_MAX_DLZKA];
    bzero(msg, MESSAGE_MAX_DLZKA);
    if (log == 1) {

        for (int i = 0; i < KLIENTI_MAX_POCET; ++i) {
            if (clientSockFD == klienti[i]->sockfd) {
                strcpy(klienti[i]->name, login);
                break;
            }
        }
        strcpy(msg, USPESNE_PRIHLASENIE);

    } else {
        strcpy(msg, NEUSPESNE_PRIHLASENIE);

    }
    sifrujRetazec(msg, msg);
    writeToClient(msg, clientSockFD);
}

void spracovanieRegistracie(int clientSockFD) {
    char *login;
    char *heslo;
    char *potvrdeneHeslo;

    login = strtok(NULL, " ");
    heslo = strtok(NULL, " ");
    potvrdeneHeslo = strtok(NULL, "/0");

    int reg = registracia(login, heslo, potvrdeneHeslo);

    char msg[MESSAGE_MAX_DLZKA];
    bzero(msg, MESSAGE_MAX_DLZKA);
    if (reg == 1) {
        for (int i = 0; i < KLIENTI_MAX_POCET; ++i) {
            if (clientSockFD == klienti[i]->sockfd) {
                strcpy(klienti[i]->name, login);
                break;
            }
        }
        strcpy(msg, USPESNA_REGISTRACIA);

    } else {
        strcpy(msg, NEUSPESNA_REGISTRACIA);

    }
    sifrujRetazec(msg, msg);
    writeToClient(msg, clientSockFD);

}

void spracovanieChatovania(int clientSockFD) {
    // mojeMeno admin text spravy
    // ODOSIELATEL PRIJIMATEL SPRAVA

    char *odosielatel;
    odosielatel = strtok(NULL, " ");

    //admin text spravy
    char *prijemca;
    prijemca = strtok(NULL, " ");
    //text spravy
    char *sprava;
    sprava = strtok(NULL, "/0");
    printf("\n\033[32;1mSERVER: Bola prijata sprava:\033[0m %s\n", sprava);

    char msg[MESSAGE_MAX_DLZKA];
    bzero(msg, MESSAGE_MAX_DLZKA);
    strcpy(msg, SPRAVA_ODOSIELATELOVI);
    sifrujRetazec(msg, msg);
    writeToClient(msg, clientSockFD);

    char pomocnyBuffer[BUFFER_SIZE];
    bzero(pomocnyBuffer, BUFFER_SIZE);
    strcat(pomocnyBuffer, SPRAVA_PRIJIMATELOVI);
    strcat(pomocnyBuffer, " ");
    strcat(pomocnyBuffer, odosielatel);
    strcat(pomocnyBuffer, " ");
    strcat(pomocnyBuffer, sprava);
    strcat(pomocnyBuffer, "\n");
    send_message(pomocnyBuffer, prijemca);
}

void spracovanieZruseniaUctu(int clientSockFD) {
    char *login;
    char *heslo;

    login = strtok(NULL, " ");
    heslo = strtok(NULL, "/0");

    int del = zrusenieUctu(login, heslo);

    char msg[MESSAGE_MAX_DLZKA];
    bzero(msg, MESSAGE_MAX_DLZKA);
    if (del == 1) {
        strcpy(msg, USPESNE_ZRUSENIE);

    } else {
        strcpy(msg, NEUSPESNE_ZRUSENIE);
    }

    sifrujRetazec(msg, msg);
    writeToClient(msg, clientSockFD);
}

void writeToClient(char *buffer, int sockfd) {
    int n = write(sockfd, buffer, strlen(buffer) + 1);
    if (n < 0) {
        perror("Error writing to socket");
    }
}

void listenToClient(char *buffer, int sockfd) {
    bzero(buffer, 256);
    int n = read(sockfd, buffer, 255);
    if (n < 0) {
        perror("Error reading from socket");
    }
}

void zoznamOnlinePouzivatelov(int clientSockFD) {
    char buffer[BUFFER_SIZE];
    bzero(buffer, 256);
    strcat(buffer, ZOZNAM_ONLINE_UZIVATELOV);
    strcat(buffer, "  ");

    int pocetOnline = 0;
    for (int i = 0; i < KLIENTI_MAX_POCET; i++) {
        if (klienti[i] && klienti[i]->name[0] != 0) {
            pocetOnline++;
            strcat(buffer, " ");
            strcat(buffer, klienti[i]->name);
        }
    }
    buffer[25] = pocetOnline + '0';
    sifrujRetazec(buffer, buffer);
    writeToClient(buffer, clientSockFD);
}

void oznamenieOPriatelstve(int clientSockFD) {
    char *komu;
    char *odKoho;
    komu = strtok(NULL, " ");
    odKoho = strtok(NULL, "/0");
    char msg[BUFFER_SIZE];
    bzero(msg, BUFFER_SIZE);

    strcat(msg, PRIDANIE_PRIATELA);
    strcat(msg, " ");
    strcat(msg, odKoho);


    int socket = najdiSocketPodlaMena(komu);
    if (!socket) {
        char newMsg[BUFFER_SIZE];
        bzero(newMsg, BUFFER_SIZE);
        strcat(newMsg, NEMOZNO_SPRIATELIT);
        strcat(newMsg, " ");
        strcat(newMsg, komu);
        sifrujRetazec(newMsg, newMsg);
        writeToClient(newMsg, clientSockFD);
    } else {
        sifrujRetazec(msg, msg);
        writeToClient(msg, socket);
    }
}

void oznamenieOOdstraneniZPriatelov(int clientSockFD) {
    char *komu;
    char *odKoho;
    komu = strtok(NULL, " ");
    odKoho = strtok(NULL, "/0");
    char msg[BUFFER_SIZE];
    bzero(msg, BUFFER_SIZE);

    strcat(msg, ODSTRANENIE_PRIATELA);
    strcat(msg, " ");
    strcat(msg, odKoho);


    int socket = najdiSocketPodlaMena(komu);
    if (!socket) {
        char newMsg[BUFFER_SIZE];
        bzero(newMsg, BUFFER_SIZE);
        strcat(newMsg, NEZRUSENIE_PRIATELSTVA);
        strcat(newMsg, " ");
        strcat(newMsg, komu);
        sifrujRetazec(newMsg, newMsg);
        writeToClient(newMsg, clientSockFD);
    } else {
        sifrujRetazec(msg, msg);
        writeToClient(msg, socket);
    }
}

int najdiSocketPodlaMena(char *meno) {
    pthread_mutex_lock(&clients_mutex);
    int sock = 0;
    for (int i = 0; i < KLIENTI_MAX_POCET; i++) {
        if (klienti[i]) {
            if (strcmp(klienti[i]->name, meno) == 0) {
                sock = klienti[i]->sockfd;
                break;

            }
        }
    }
    pthread_mutex_unlock(&clients_mutex);
    return sock;
}


