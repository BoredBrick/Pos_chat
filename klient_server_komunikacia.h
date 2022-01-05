//
// Created by Danko on 04/01/2022.
//

#ifndef POS_CHAT_KLIENT_SERVER_KOMUNIKACIA_H
#define POS_CHAT_KLIENT_SERVER_KOMUNIKACIA_H

#endif //POS_CHAT_KLIENT_SERVER_KOMUNIKACIA_H
#include <stdio.h>
#include <string.h>
#include <unistd.h>

void writeToSocket(char *buffer, int sockfd) {
    int n = write(sockfd, buffer, strlen(buffer));
    if (n < 0) {
        perror("Error writing to socket");
    }
}

void listenToServer(char *buffer, int sockfd) {
    bzero(buffer, 256);
    int n = read(sockfd, buffer, 256 - 1);
    if (n < 0) {
        perror("Error reading from socket");
    }
}