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

int main(int argc, char *argv[])
{
    FILE *fp;

    fp = fopen("file4.txt", "w+");

    fputs("This is c programming.", fp);
    fputs("This is a system programming language.", fp);

    fclose(fp);

    return(0);
    return 1;

    int sockfd, newsockfd;
    socklen_t cli_len;
    struct sockaddr_in serv_addr, cli_addr;
    int n;
    char buffer[256];

    if (argc < 2)
    {
        fprintf(stderr,"usage %s port\n", argv[0]);
        return 1;
    }

    bzero((char*)&serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(atoi(argv[1]));

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
    {
        perror("Error creating socket");
        return 1;
    }

    if (bind(sockfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0)
    {
        perror("Error binding socket address");
        return 2;
    }
    listen(sockfd, 5);

    //nadviazeme spojenie s jednym klientskym socketom
    cli_len = sizeof(cli_addr);
    newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &cli_len);
    if (newsockfd < 0) {
        perror("ERROR on accept");
        return 3;
    }

    //while(true) {

        bzero(buffer, 256);
        n = read(newsockfd, buffer, 255);
        if (n < 0) {
            perror("Error reading from socket");
            return 4;
        }
        printf("Here is the message: %s\n", buffer);

        registracia("meno", "heslo", "heslo");

        const char *msg = "I got your message";
        n = write(newsockfd, msg, strlen(msg) + 1);
        if (n < 0) {
            perror("Error writing to socket");
            return 5;
        }
  //  }

    close(newsockfd);
    close(sockfd);

    return 0;
}
int registracia(char* login, char* heslo, char* potvrdHeslo) {
    FILE *subor;
    subor = fopen("zaregistrovani_pouzivatelia.txt", "w");
    fprintf(subor,"AAA");
    fclose(subor);
    return 1;

    bool jeUzZaregistrovany = false;
//    while (subor != EOF) {
//        if (strcmp((const char *) fscanf(subor, "%s"), login) == 0) {
//            jeUzZaregistrovany = true;
//            break;
//        }
//    }

    if (!jeUzZaregistrovany) {
        fprintf(subor, login);
        fprintf(subor, " ");
        fprintf(subor, potvrdHeslo);
        fprintf(subor, "\n");
        fclose(subor);
        return 1;

    } else {
// Zadany login sa v databaze pouzivatelov uz nachadza
        return 0;
    }
}