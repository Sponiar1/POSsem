#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>

void *userInteraction(int newsockfd) {
    int n;
    char buffer[256];
    char vyzva[50];

    bzero(vyzva, 50);
    strcpy(vyzva, "Meno: ");
    n = write(newsockfd, vyzva, strlen(vyzva) + 1);
    n = read(newsockfd, buffer, 255);
    char meno[50];
    bzero(meno, 50);
    strcpy(meno, buffer);
    printf("%s \n", meno);
    int a = 10;
    bzero(buffer, 256);
    bzero(vyzva, 50);
    strcpy(vyzva, "Heslo: ");
    n = write(newsockfd, vyzva, strlen(vyzva) + 1);
    n = read(newsockfd, buffer, 255);
    char heslo[50];
    bzero(heslo, 50);
    strcpy(heslo, buffer);
    printf("%s \n", heslo);
    int logged = 0;
    logged = login(meno, heslo);
    while (logged==1) {
        bzero(buffer, 256);
        n = read(newsockfd, buffer, 255);
        if (n < 0) {
            perror("Error reading from socket");
            return 4;
        }

        printf("Here is the message: %s\n", buffer);
        if (strncmp("end", buffer, 3) == 0) {
            break;
        }
        const char *msg = "I got your message";
        n = write(newsockfd, msg, strlen(msg) + 1);

        if (n < 0) {
            perror("Error writing to socket");
            return 5;
        }
    }
    close(newsockfd);
}

int main(int argc, char *argv[])
{
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
    cli_len = sizeof(cli_addr);
    bzero(buffer, 256);

    while(1==1) {
        newsockfd = accept(sockfd, (struct sockaddr*)&cli_addr, &cli_len);
        if (newsockfd < 0)
        {
            perror("ERROR on accept");
            return 3;
        }
        pthread_t client;
        pthread_create(&client, NULL, &userInteraction, newsockfd);
    }
    close(sockfd);

    return 0;
}

int login(char meno[], char heslo[]) {
    int found;
    found = 0;
    FILE *f;
    f = fopen("/tmp/PosSemTest2/login.txt", "r");
    char fileMeno[50];
    char fileHeslo[50];
    bzero(fileMeno, 50);
    bzero(fileHeslo, 50);
    meno[strcspn(meno, "\n")] = 0;
    heslo[strcspn(heslo, "\n")] = 0;
    while(!feof(f)) {
        fscanf(f, "%s", fileMeno);
        printf("%s \n", fileMeno);
        fscanf(f, "%s", fileHeslo);
        printf("%s \n", fileHeslo);
        if (strncmp(fileMeno, meno, strlen(meno)) == 0 && (strncmp(fileHeslo, heslo, strlen(heslo))) == 0) {
            found = 1;
            printf("Našiel som");
            break;
        }
    }

    fclose(f);
    return found;
}

/*
        for (int i = 0; buffer[i] != '\0'; ++i) {
            buffer[i] += 3;
        }
        printf("Here is caesar message: %s\n", buffer);
        */