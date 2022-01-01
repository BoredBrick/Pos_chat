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

#define BUFFER_SIZE 256
#define NAME_LENGTH 30
#define MAX_CLIENTS 10

static _Atomic unsigned int cli_count = 0;
static int clientIDstart = 10;

// CLIENT STRUCTURE

typedef struct {
    struct sockaddr_in address;
    int sockfd;
    int clientID;
    char name[NAME_LENGTH];
} client_t;
client_t *clients[MAX_CLIENTS];

pthread_mutex_t clients_mutex = PTHREAD_MUTEX_INITIALIZER;

// DOLEZITE AS FUCK No.1 (add Client to queue)
void queue_add(client_t *client) {
    pthread_mutex_lock(&clients_mutex);

    for (int i = 0; i < MAX_CLIENTS; i++) {
        if (!clients[i]) {
            clients[i] = client;
            break;
        }
    }

    pthread_mutex_unlock(&clients_mutex);
}

// DOLEZITE AS FUCK No.2 (remove Client from queue)
void queue_remove(int clientID) {
    pthread_mutex_lock(&clients_mutex);

    for (int i = 0; i < MAX_CLIENTS; i++) {
        if (clients[i]) {
            if (clients[i]->clientID == clientID) {
                clients[i] = NULL;
                break;
            }
        }
    }
    pthread_mutex_unlock(&clients_mutex);
}

//posle spravu konkretnemu clientID
void send_message(char *s, char *komu) {
    pthread_mutex_lock(&clients_mutex);
    for (int i = 0; i < MAX_CLIENTS; i++) {
        if (clients[i]) {
            if (strcmp(clients[i]->name,komu) == 0) {
                printf("Zapisujem na socket: %d\n", clients[i]->sockfd);
                if (write(clients[i]->sockfd, s, strlen(s)) < 0) {
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
    printf("Bolo vytvorene vlakno pre klienta!\n");
    char buffer[BUFFER_SIZE];
    cli_count++;

    client_t *cli = (client_t *) arg;

    printf("Pripojil sa klient so socketom: %d\n", cli->sockfd);
    /*
    *      // RECEIVING NAME FROM THE CLIENT
    *      } else {
    *          // CLIENT IS JOINING THE SERVER
    *          strcpy(cli->name, name);
    *          sprintf(buffer, "%s has joined\n", cli->name);
    *          printf("%s", buffer);
    *          send_message(buffer, cli->clientID);
    *      }
    *      bzero(buffer, BUFFER_SIZE);
    *
    *      while(1) {
    *          if (leave_flag) {
    *              break;
    *          }
    *
    *          int receive = recv(cli->sockfd, buffer, BUFFER_SIZE, 0);
    *
    *          if (receive > 0) {
    *              if (strlen(buffer) > 0) {
    *                  send_message(buffer, cli->clientID);
    *                  str_trim_lf(buffer, strlen(buffer));
    *                  printf("%s\n", buffer);
    *
    *              }
    *          } else if (receive == 0 || strcmp(buffer, "exit") == 0) {
    *              sprintf(buffer, "%s has left\n", cli->name);
    *              printf("%s", buffer);
    *              send_message(buffer, cli->clientID);
    *              leave_flag = 1;
    *          } else {
    *              printf("ERROR: -1\n");
    *              leave_flag = 1;
    *          }
    *           bzero(buffer, BUFFER_SIZE);
    *      }
    */

    int bolExit = 0;
    int n;
    int clientSockFD = cli->sockfd;

    while (bolExit == 0) {
        listenToClient(buffer, clientSockFD);
        char *typSpravy;
        typSpravy = strtok(buffer, " ");

        if (strcmp(typSpravy, "registracia") == 0) {
            spracovanieRegistracie(clientSockFD);

        } else if (strcmp(typSpravy, "prihlasenie") == 0) {
            spracovaniePrihlasenia(clientSockFD);

        } else if (strcmp(typSpravy, "chatovanie") == 0) {
            spracovanieChatovania(clientSockFD);

        } else if (strcmp(typSpravy, "zrusenie") == 0) {
            spracovanieZruseniaUctu(clientSockFD);

        } else if (strcmp(typSpravy, "exit") == 0) {
            bolExit = 1;

        } else if (strcmp(typSpravy, "nickname") == 0) {
            // int res =
            char* meno;
            meno = strtok(NULL, "/0");

//            strcpy(cli->name, meno);
//            sprintf(buffer, "\n%s has joined\n", cli->name);
//            printf("%s", buffer);
//            send_message(buffer, cli->clientID);

        }
    }

    close(clientSockFD);
    queue_remove(cli->clientID);
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
         *          printf("Maximum clients connected. Connection rejected.\n");
         *          print_ip_addr(cli_addr);
         *          close(clientSockFD);
         *          continue;
         *      }
         */
        // CLIENT SETTINGS
        client_t *cli = (client_t *) malloc(sizeof(client_t));
        cli->address = cli_addr;
        cli->sockfd = clientSockFD;
        cli->clientID = clientIDstart++;

        queue_add(cli);

        pthread_t vlakno;
        pthread_create(&vlakno, NULL, &obsluhaKlienta, (void *) cli);

        // REDUCE CPU USAGE
        sleep(1);

    }


    close(sockfd);

    return 0;
}

void spracovaniePrihlasenia(int newsockfd) {
    char *login;
    char *heslo;

    login = strtok(NULL, " ");
    heslo = strtok(NULL, "/0");

    int log = prihlasenie(login, heslo);

    char *msg;
    if (log == 1) {
        //msg = "\n\033[32;1mSERVER: Uspesne prihlasenie.\033[0m\n";
        msg = "LOGINTRUE";

        for (int i = 0; i < MAX_CLIENTS; ++i) {
            if (newsockfd == clients[i]->sockfd) {
                strcpy(clients[i]->name, login);
                break;
            }
        }

    } else {
        //msg = "\n\033[32;1mSERVER: Neuspesne prihlasenie. Nespravne meno alebo heslo.\033[0m\n";
        msg = "LOGINFALSE";
    }
    writeToClient(msg,newsockfd);

}

void spracovanieRegistracie(int newsockfd) {
    char *login;
    char *heslo;
    char *potvrdeneHeslo;

    login = strtok(NULL, " ");
    heslo = strtok(NULL, " ");
    potvrdeneHeslo = strtok(NULL, "/0");

    int reg = registracia(login, heslo, potvrdeneHeslo);

    char *msg;
    if (reg == 1) {
        //msg = "\n\033[32;1mSERVER: Uspesna registracia.\033[0m\n";
        msg = "REGTRUE";
        for (int i = 0; i < MAX_CLIENTS; ++i) {
            if (newsockfd == clients[i]->sockfd) {
                strcpy(clients[i]->name, login);
                break;
            }
        }
    } else if (reg == 0) {
        //msg = "\n\033[32;1mSERVER: Neuspesna registracia. Zadany login uz existuje.\033[0m\n";
        msg = "REGFALSE";
    } else {
        //msg = "\n\033[32;1mSERVER: Neuspesna registracia. Hesla sa nezhoduju.\033[0m\n";
        msg = "REGFALSE";
    }
    writeToClient(msg,newsockfd);

}

void spracovanieChatovania(int newsockfd) {
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

//    char *buffer;
//    strcat(buffer, "Dostali ste spravu: ");
//    strcat(buffer, sprava);

    //char *msg = "\n\033[32;1mSERVER: Sprava bola obdrzana.\033[0m\n";
    char *msg = "SPRAVA";
    writeToClient(msg,newsockfd);

    char pomocnyBuffer[BUFFER_SIZE];
    strcat(pomocnyBuffer, "USER_SPRAVA");
    strcat(pomocnyBuffer, " ");
    strcat(pomocnyBuffer, odosielatel);
    strcat(pomocnyBuffer, " ");
    strcat(pomocnyBuffer, sprava);

    send_message(pomocnyBuffer, prijemca);

}

void spracovanieZruseniaUctu(int newsockfd) {
    char *login;
    char *heslo;

    login = strtok(NULL, " ");
    heslo = strtok(NULL, "/0");

    int del = zrusenieUctu(login, heslo);
    char *msg;
    if (del == 1) {
        msg = "\n\033[32;1mSERVER: Uspesne zrusenie uctu.\033[0m\n";
    } else {
        msg = "\n\033[32;1mSERVER: Neuspesne zrusenie uctu. Nespravne meno alebo heslo.\033[0m\n";
    }
    writeToClient(msg,newsockfd);

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





