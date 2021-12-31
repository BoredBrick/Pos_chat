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
#include <pthread.h>

#define BUFFER_SIZE 256
#define NAME_LENGTH 30
#define MAX_CLIENTS 10

static _Atomic unsigned int cli_count = 0;
static int uid = 10;

// CLIENT STRUCTURE

typedef struct {
    struct sockaddr_in address;
    int sockfd;
    int uid;
    char name[NAME_LENGTH];
} client_t;
client_t *clients[MAX_CLIENTS];

pthread_mutex_t clients_mutex = PTHREAD_MUTEX_INITIALIZER;

/* void str_overwrite_stdout() {
*      printf("\r%s", "> ");
*      fflush(stdout);
* }
*
* void str_trim_lf(char* arr, int length) {
*      for (int i=0; i<length; i++) {
*          if (arr[i] == '\n') {
*              arr[i] = '\0';
*              break;
*          }
*      }
* }
*/
// DOLEZITE AS FUCK No.1 (add Client to queue)
void queue_add(client_t *cl) {
    pthread_mutex_lock(&clients_mutex);

    for (int i = 0; i < MAX_CLIENTS; i++) {
        if (!clients[i]) {
            clients[i] = cl;
            break;
        }
    }

    pthread_mutex_unlock(&clients_mutex);
}

// DOLEZITE AS FUCK No.2 (remove Client from queue)
void queue_remove(int uid) {
    pthread_mutex_lock(&clients_mutex);

    for (int i = 0; i < MAX_CLIENTS; i++) {
        if (clients[i]) {
            if (clients[i]->uid == uid) {
                clients[i] = NULL;
                break;
            }
        }
    }
    pthread_mutex_unlock(&clients_mutex);
}


/*
 * void print_ip_addr(struct sockaddr_in addr) {
 *      printf("%d.%d.%d.%d",
 *          addr.sin_addr.s_addr & 0xff,
 *          (addr.sin_addr.s_addr & 0xff00) >> 8,
 *          (addr.sin_addr.s_addr & 0xff0000) >> 16,
 *          (addr.sin_addr.s_addr & 0xff000000) >> 24);
 * }
 */
// (send message to all clients except me)
void send_message(char *s, int uid) {
    pthread_mutex_lock(&clients_mutex);
    for (int i = 0; i < MAX_CLIENTS; i++) {
        if (clients[i]) {
            if (clients[i]->uid != uid) {
                if (write(clients[i]->sockfd, s, strlen(s)) < 0) {
                    printf("ERROR: write to descriptor failed\n");
                    break;
                }
            }
        }
    }
    pthread_mutex_unlock(&clients_mutex);
}


void *handle_client(void *arg) {
    printf("I CAN HANDLE THIS!\n");
    char buffer[BUFFER_SIZE];
    char name[NAME_LENGTH];
    int leave_flag = 0; // was client connected?
    cli_count++;

    client_t *cli = (client_t *) arg;
    /*
    *      // RECEIVING NAME FROM THE CLIENT
    *      } else {
    *          // CLIENT IS JOINING THE SERVER
    *          strcpy(cli->name, name);
    *          sprintf(buffer, "%s has joined\n", cli->name);
    *          printf("%s", buffer);
    *          send_message(buffer, cli->uid);
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
    *                  send_message(buffer, cli->uid);
    *                  str_trim_lf(buffer, strlen(buffer));
    *                  printf("%s\n", buffer);
    *
    *              }
    *          } else if (receive == 0 || strcmp(buffer, "exit") == 0) {
    *              sprintf(buffer, "%s has left\n", cli->name);
    *              printf("%s", buffer);
    *              send_message(buffer, cli->uid);
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
    int newsockfd = cli->sockfd;

    while (bolExit == 0) {
        bzero(buffer, 256);
        n = read(newsockfd, buffer, 255);
        //citanie buffra co prisiel na socket
        if (n < 0) {
            perror("Error reading from socket");
            //return 4;
        }
        char *typSpravy;
        typSpravy = strtok(buffer, " ");

        if (strcmp(typSpravy, "registracia") == 0) {
            int res = spracovanieRegistracie(newsockfd, n);

        } else if (strcmp(typSpravy, "prihlasenie") == 0) {
            int res = spracovaniePrihlasenia(newsockfd, n);

        } else if (strcmp(typSpravy, "chatovanie") == 0) {
            int res = spracovanieChatovania(newsockfd, n);

        } else if (strcmp(typSpravy, "zrusenie") == 0) {
            int res = spracovanieZruseniaUctu(newsockfd, n);

        } else if (strcmp(typSpravy, "exit") == 0) {
            bolExit = 1;

        } else if (strcmp(typSpravy, "nickname") == 0) {
            // int res =
            char* meno;
            meno = strtok(NULL, "/0");

//            strcpy(cli->name, meno);
//            sprintf(buffer, "\n%s has joined\n", cli->name);
//            printf("%s", buffer);
//            send_message(buffer, cli->uid);

        }
    }

    close(newsockfd);
    queue_remove(cli->uid);
    free(cli);
    cli_count--;
    pthread_detach(pthread_self());
    return NULL;
}


int main(int argc, char *argv[]) {
    int sockfd, newsockfd;
    socklen_t cli_len;
    // server address, client address
    struct sockaddr_in serv_addr, cli_addr;
    int n;
    char buffer[256];
    pthread_t vlakno;

    if (argc < 2) {
        fprintf(stderr, "usage %s port\n", argv[0]);
        return 1;
    }

    // SOCKET SETTINGS
    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(atoi(argv[1]));

    // SIGNALS
    // signal(SIGPIPE, SIG_IGN);

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
        newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &cli_len);
        if (newsockfd < 0) {
            perror("ERROR on accept");
            return 3;
        }
        /*      // CHECK FOR MAX_CLIENTS
         *      if ((cli_count + 1) == MAX_CLIENTS) {
         *          printf("Maximum clients connected. Connection rejected.\n");
         *          print_ip_addr(cli_addr);
         *          close(newsockfd);
         *          continue;
         *      }
         */
        // CLIENT SETTINGS
        client_t *cli = (client_t *) malloc(sizeof(client_t));
        cli->address = cli_addr;
        cli->sockfd = newsockfd;
        cli->uid = uid++;

        // ADD CLIENT TO QUEUE
        queue_add(cli);
        pthread_create(&vlakno, NULL, &handle_client, (void *) cli);

        // REDUCE CPU USAGE
        sleep(1);

    }


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

int spracovanieZruseniaUctu(int newsockfd, int n) {
    char *login;
    char *heslo;

    login = strtok(NULL, " ");
    heslo = strtok(NULL, "/0");

    printf("LOGIN: %s\n", login);
    printf("HESLO: %s\n", heslo);

    int del = zrusenieUctu(login, heslo);

    printf("VYSLEDOK ZRUSENIA UCTU: %d\n", del);

    if (del == 1) {
        const char *msg = "\n\033[32;1mSERVER: Uspesne zrusenie uctu.\033[0m\n";
        n = write(newsockfd, msg, strlen(msg) + 1);
        if (n < 0) {
            perror("Error writing to socket");
            return 5;
        }
    } else {
        const char *msg = "\n\033[32;1mSERVER: Neuspesne zrusenie uctu. Nespravne meno alebo heslo.\033[0m\n";
        n = write(newsockfd, msg, strlen(msg) + 1);
        if (n < 0) {
            perror("Error writing to socket");
            return 5;
        }
    }
    return 0;
}



