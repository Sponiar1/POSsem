#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int main(int argc, char *argv[])
{
    int sockfd, n, logged;
    struct sockaddr_in serv_addr;
    struct hostent* server;
    int on = 1;
    char buffer[256];
    char name[50];

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


    while (on==1) {
    while (logged == 0) {
        printf("Vitajte zvolte akciu: \n");
        printf("Prihlasit sa - 1\n");
        printf("Registrovat sa - 2\n");
        fgets(buffer, 255, stdin);
        n = write(sockfd, buffer, strlen(buffer));

        if (strncmp("1", buffer, 1) == 0) {
            bzero(buffer, 256);
            n = read(sockfd, buffer, 255);
            printf("%s", buffer);
            fgets(buffer, 255, stdin);
            n = write(sockfd, buffer, strlen(buffer));
            bzero(name, 50);
            strcpy(name, buffer);

            bzero(buffer, 256);
            n = read(sockfd, buffer, 255);
            printf("%s", buffer);
            fgets(buffer, 255, stdin);
            n = write(sockfd, buffer, strlen(buffer));
            n = read(sockfd, buffer, 255);
        }
        if (strncmp("2", buffer, 1) == 0) {
            bzero(buffer, 256);
            n = read(sockfd, buffer, 255);
            printf("%s", buffer);
            fgets(buffer, 255, stdin);
            n = write(sockfd, buffer, strlen(buffer));

            bzero(buffer, 256);
            n = read(sockfd, buffer, 255);
            printf("%s", buffer);
            fgets(buffer, 255, stdin);
            n = write(sockfd, buffer, strlen(buffer));
            n = read(sockfd, buffer, 255);
        }
        printf("%s", buffer);
        if(strncmp("V", buffer, 1) == 0) {
            logged = 1;
            printf("\n");
        }
    }

    while (logged==1) {
        printf("Zvolte akciu: \n");
        printf("1 - Free writing: \n");
        printf("2 - Kontakty: \n");
        printf("3 - Pridat novy kontakt: \n");
        printf("4 - Potvrdit ziadosti o kontakt\n");
        printf("5 - Odhlasit \n");
        printf("6 - Zrusit ucet \n");
        printf("7 - Vypnut \n");
        bzero(buffer, 256);
        fgets(buffer, 255, stdin);
        int menu = 1;
        menu = atoi(buffer);
        //printf("%d", menu);
        n = write(sockfd, buffer, strlen(buffer));
        switch (menu) {
            case 1:
                while (1 == 1) {
                    printf("Please enter a message: ");
                    bzero(buffer, 256);
                    fgets(buffer, 255, stdin);

                    n = write(sockfd, buffer, strlen(buffer));
                    if (n < 0) {
                        perror("Error writing to socket");
                        return 5;
                    }

                    if (strncmp("end", buffer, 3) == 0) {
                        break;
                    }

                    bzero(buffer, 256);
                    n = read(sockfd, buffer, 255);
                    if (n < 0) {
                        perror("Error reading from socket");
                        return 6;
                    }

                    printf("%s\n", buffer);
                }
                break;
            case 2:
                //printf("Vybral som kontakty");
                while (1 == 1) {
                    n = read(sockfd, buffer, 255);
                    if (strncmp("Done", buffer, 4) == 0) {
                        //printf("idem preč z whilu");
                        break;
                    }
                    printf("%s\n", buffer);
                }
                break;
            case 3:
                printf("Chcem sa spojit s:");
                fgets(buffer, 255, stdin);
                n = write(sockfd, buffer, strlen(buffer));
                usleep(5);
                n = read(sockfd, buffer, 255);
                printf("%s", buffer);
                break;
            case 4:
                while (1 == 1) {
                    n = read(sockfd, buffer, 255);
                    if (strncmp("Done", buffer, 4) == 0) {
                        //printf("idem preč z whilu");
                        break;
                    }
                    printf("%s\n", buffer);
                }
                printf("Chcete si niekoho pridať? meno = pridanie / 0 = odísť\n");
                bzero(buffer, 256);
                fgets(buffer, 255, stdin);
                n = write(sockfd, buffer, strlen(buffer));
                break;
            case 5:
                logged = 0;
                break;
            case 6:
                printf("vyfilovane");
                logged = 0;
                break;
            case 7:
                logged = 0;
                on = 0;
                break;
            default:
                break;
        }
    }
    }
    close(sockfd);

    return 0;
}