//
// Created by Matúš Baláži on 28. 12. 2021.
//

#include "klient.h"

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>

int main(int argc, char *argv[])
{
    int sockfd, n;
    struct sockaddr_in serv_addr;
    struct hostent* server;
    char buffer[256];

    if (argc < 3)
    {
        fprintf(stderr,"usage %s hostname port\n", argv[0]);
        return 1;
    }

    server = gethostbyname(argv[1]);
    if (server == NULL)
    {
        fprintf(stderr, "Error, no such host\n");
        return 2;
    }

    bzero((char*)&serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy(
            (char*)server->h_addr,
            (char*)&serv_addr.sin_addr.s_addr,
            server->h_length
    );
    serv_addr.sin_port = htons(atoi(argv[2]));

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
    {
        perror("Error creating socket");
        return 3;
    }

    if(connect(sockfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0)
    {
        perror("Error connecting to socket");
        return 4;
    }

    int bolExit = 0;
    char* exit = "exit\n";
    int bolaRegistracia = 0;

    while(bolExit == 0) {
        int akcia = 1;

        puts("\033[36;1m|--- CHAT APP ---|\033[0m");
        puts("[1] Registracia");
        puts("[2] Chatovanie");
        puts("[0] Koniec");
        printf("\n\033[35;1mKLIENT: Zadajte akciu: \033[0m");
        scanf("%d", &akcia);
        getchar();

        if (akcia == 1) {
            while (bolaRegistracia == 0) {
                char login[30], heslo[30];
                bzero(buffer, 256);
                printf("\n\033[35;1mKLIENT: Zadajte login: \033[0m");
                scanf("%s", &login);
                getchar();
                printf("\n\033[35;1mKLIENT: Zadajte heslo: \033[0m");
                scanf("%s", &heslo);
                getchar();
                //memset()
                strcat(buffer, login);
                strcat(buffer, " ");
                strcat(buffer, heslo);
                printf("%s", buffer);


                //gets(buffer, 255, stdin);

                buffer[strcspn(buffer, "\n")] = 0;

                bolaRegistracia = 1;
                n = write(sockfd, buffer, strlen(buffer));
                if (n < 0) {
                    perror("Error writing to socket");
                    return 5;
                }

                bzero(buffer, 256);
                n = read(sockfd, buffer, 255);
                if (n < 0) {
                    perror("Error reading from socket");
                    return 6;
                }
                printf("%s\n", buffer);
            }
        } else if (akcia == 2) {
            printf("\n\033[35;1mKLIENT: Prosim, zadajte spravu: \033[0m");
            bzero(buffer, 256);
            fgets(buffer, 255, stdin);

            n = write(sockfd, buffer, strlen(buffer));
            if (n < 0) {
                perror("Error writing to socket");
                return 5;
            }

            if (strcmp(buffer, exit) == 0) {
                bolExit = 1;
            }

            bzero(buffer, 256);
            n = read(sockfd, buffer, 255);
            if (n < 0) {
                perror("Error reading from socket");
                return 6;
            }

            printf("%s\n", buffer);
        } else if (akcia == 0) {
            bolExit = 1;
            n = write(sockfd, "exit\n", strlen("exit\n"));
            if (n < 0) {
                perror("Error writing to socket");
                return 5;
            }
        }
    }

    close(sockfd);

    return 0;
}