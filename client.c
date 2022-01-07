#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
//zakódovanie správy
char* code(char message[]) {
    for (int i = 0; message[i] != '\0'; ++i) {
        message[i] += 5;
    }
    return message;
}
//dekódovanie správy
char* decode(char message[]) {
    for (int i = 0; message[i] != '\0'; ++i) {
        message[i] -= 5;
    }
    return message;
}
/*
int sendFile(char nameOfFile[], int newsockfd) {
    int n, maxFileSize;
    maxFileSize = 5*1024*1024;
    long long fileSize = 0;
    FILE *f;
    nameOfFile[strcspn(nameOfFile, "\n")] = 0;
    char path[512];
    char data[maxFileSize];
    bzero(path, 512);
    bzero(data, maxFileSize);
    strcpy(path,"/tmp/PosSemTest/");
    strcat(path, nameOfFile);
    printf("%s", path);
    f = fopen(path, "r");
    if (f == NULL) {
        fprintf(stderr, "Nemožno otvoriť súbor\n");
        return 1;
    }
    fseek(f, 0, SEEK_END);
    fileSize = ftell(f);
    printf("%d", fileSize);
    if (fileSize <= maxFileSize) {
        n = write(newsockfd, nameOfFile, strlen(nameOfFile));
        while (!feof(f)) {
            bzero(data, maxFileSize);
            fgets(data, maxFileSize, f);
            n = write(newsockfd, data, strlen(data) - 1);
        }
    } else {
        strcpy(nameOfFile, "Error");
        n = write(newsockfd, nameOfFile, strlen(nameOfFile));
        return 2;
    }
    bzero(data, maxFileSize);
    strcpy(data, "Done");
    n = write(newsockfd, data, strlen(data));
    fclose(f);
    return 0;
}
*/
int main(int argc, char *argv[])
{
    int sockfd, n, logged;
    struct sockaddr_in serv_addr;
    struct hostent* server;
    int on = 1;
    int menu;
    char buffer[256];
    char name[20];

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
        if (n < 0) {
            perror("Error writing to socket");
            return 5;
        }

        if (strncmp("1", buffer, 1) == 0) {
            bzero(buffer, 256);
            n = read(sockfd, buffer, 255);
            if (n < 0) {
                perror("Error reading from socket");
                return 6;
            }
            printf("%s", buffer);
            bzero(buffer, 256);
            fgets(buffer, 255, stdin);
            n = write(sockfd, buffer, strlen(buffer));
            if (n < 0) {
                perror("Error writing to socket");
                return 5;
            }
            bzero(name, 20);
            strcpy(name, buffer);

            bzero(buffer, 256);
            n = read(sockfd, buffer, 255);
            if (n < 0) {
                perror("Error reading from socket");
                return 6;
            }
            printf("%s", buffer);
            bzero(buffer, 256);
            fgets(buffer, 255, stdin);
            n = write(sockfd, buffer, strlen(buffer));
            if (n < 0) {
                perror("Error writing to socket");
                return 5;
            }
            n = read(sockfd, buffer, 255);
            if (n < 0) {
                perror("Error reading from socket");
                return 6;
            }
        }
        if (strncmp("2", buffer, 1) == 0) {
            bzero(buffer, 256);
            n = read(sockfd, buffer, 255);
            if (n < 0) {
                perror("Error reading from socket");
                return 6;
            }
            printf("%s", buffer);
            bzero(buffer, 256);
            fgets(buffer, 255, stdin);
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
            printf("%s", buffer);
            bzero(buffer, 256);
            fgets(buffer, 255, stdin);
            n = write(sockfd, buffer, strlen(buffer));
            if (n < 0) {
                perror("Error writing to socket");
                return 5;
            }
            n = read(sockfd, buffer, 255);
            if (n < 0) {
                perror("Error reading from socket");
                return 6;
            }
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
        printf("5 - Odtranit kontakt\n");
        printf("6 - Notifications\n");
        printf("7 - Zobrazit spravy\n");
        printf("8 - Odoslat spravu\n");
        printf("9 - Odhlasit \n");
        printf("10 - Zrusit ucet \n");
        printf("11 - Vypnut \n");
        printf("12 - Send file\n");
        printf("13 - Create group chat\n");
        printf("14 - add to group chat\n");
        printf("15 - send message to group chat\n");
        printf("16 - read group chat\n");
        bzero(buffer, 256);
        fgets(buffer, 255, stdin);
        menu = atoi(buffer);
        n = write(sockfd, buffer, strlen(buffer));
        if (n < 0) {
            perror("Error writing to socket");
            return 5;
        }
        switch (menu) {
            case 1: //free writing
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
            case 2: //kontakty
                while (1 == 1) {
                    bzero(buffer, 256);
                    n = read(sockfd, buffer, 255);
                    if (n < 0) {
                        perror("Error reading from socket");
                        return 6;
                    }
                    if (strncmp("Done", buffer, 4) == 0) {
                        break;
                    }
                    printf("%s\n", buffer);
                    usleep(5);
                }
                break;
            case 3: //poslat ziadost o priatelstvo
                printf("Chcem sa spojit s:\n");
                bzero(buffer, 256);
                fgets(buffer, 255, stdin);
                n = write(sockfd, buffer, strlen(buffer));
                if (n < 0) {
                    perror("Error writing to socket");
                    return 5;
                }
                usleep(5);
                bzero(buffer, 256);
                n = read(sockfd, buffer, 255);
                if (n < 0) {
                    perror("Error reading from socket");
                    return 6;
                }
                printf("%s\n", buffer);
                break;
            case 4: //potvrdit ziadost o priatelstvo
                while (1 == 1) {
                    bzero(buffer, 256);
                    n = read(sockfd, buffer, 255);
                    if (n < 0) {
                        perror("Error reading from socket");
                        return 6;
                    }
                    if (strncmp("Done", buffer, 4) == 0) {
                        //printf("idem preč z whilu");
                        break;
                    }
                    printf("%s\n", buffer);
                    usleep(5);
                }
                printf("Chcete si niekoho pridať? meno = pridanie / 0 = odísť\n");
                bzero(buffer, 256);
                fgets(buffer, 255, stdin);
                n = write(sockfd, buffer, strlen(buffer));
                if (n < 0) {
                    perror("Error writing to socket");
                    return 5;
                }
                break;
            case 5: //vymazat z priatelov
                while (1 == 1) {
                    n = read(sockfd, buffer, 255);
                    if (n < 0) {
                        perror("Error reading from socket");
                        return 6;
                    }
                    if (strncmp("Done", buffer, 4) == 0) {
                        break;
                    }
                    printf("%s\n", buffer);
                }
                printf("Chcete si niekoho vymazat z priatelov? meno = odobranie / 0 = odísť\n");
                bzero(buffer, 256);
                fgets(buffer, 255, stdin);
                usleep(5);
                n = write(sockfd, buffer, strlen(buffer));
                if (n < 0) {
                    perror("Error writing to socket");
                    return 5;
                }
                break;
            case 6: //notifikacie
                while (1 == 1) {
                    n = read(sockfd, buffer, 255);
                    if (n < 0) {
                        perror("Error reading from socket");
                        return 6;
                    }
                    if (strncmp("Done", buffer, 4) == 0) {
                        break;
                    }
                    printf("%s\n", buffer);
                }
                break;
            case 7: //precitat spravy
                printf("Chcem si pozriet konverzaciu s : (0 = exit)\n");
                while (1 == 1) {
                    n = read(sockfd, buffer, 255);
                    if (n < 0) {
                        perror("Error reading from socket");
                        return 6;
                    }
                    usleep(5);
                    if (strncmp("Done", buffer, 4) == 0) {
                        break;
                    }
                    printf("%s\n", buffer);
                }
                bzero(buffer, 256);
                fgets(buffer, 255, stdin);
                n = write(sockfd, buffer, strlen(buffer));
                if (n < 0) {
                    perror("Error writing to socket");
                    return 5;
                }
                if (strncmp("0", buffer, 1) == 0) {
                    break;
                }
                while (1==1) {
                    n = read(sockfd, buffer, 255);
                    if (n < 0) {
                        perror("Error reading from socket");
                        return 6;
                    }
                    if(strncmp("Done", buffer, 4) == 0) {
                        break;
                    }
                    printf("%s: ", buffer);
                    usleep(5);
                    n = read(sockfd, buffer, 255);
                    if (n < 0) {
                        perror("Error reading from socket");
                        return 6;
                    }
                    strcpy(buffer, decode(buffer));
                    printf("%s", buffer);
                    usleep(5);
                }
                break;
            case 8: //napisat uzivatelovi
                printf("Chcem napisat spravu uzivatelovi: (0 = exit)\n");
                while (1 == 1) {
                    n = read(sockfd, buffer, 255);
                    if (n < 0) {
                        perror("Error reading from socket");
                        return 6;
                    }
                    usleep(5);
                    if (strncmp("Done", buffer, 4) == 0) {
                        break;
                    }
                    printf("%s\n", buffer);
                }
                bzero(buffer, 256);
                fgets(buffer, 255, stdin);
                n = write(sockfd, buffer, strlen(buffer));
                if (n < 0) {
                    perror("Error writing to socket");
                    return 5;
                }
                if (strncmp("0", buffer, 1) != 0) {
                    printf("Moja správa: ");
                    bzero(buffer, 256);
                    fgets(buffer, 255, stdin);
                    strcpy(buffer, code(buffer));
                    n = write(sockfd, buffer, 50);
                    if (n < 0) {
                        perror("Error writing to socket");
                        return 5;
                    }
                }
                break;
            case 9: //odhlasit
                logged = 0;
                break;
            case 10: //zrusit ucet
                logged = 0;
                break;
            case 11: //vypnut klienta
                logged = 0;
                on = 0;
                break;
      /*      case 12: //poslat file(nefunkcne)
                printf("Chcem poslat file uzivatelovi: (0 = exit)\n");
                while (1 == 1) {
                    n = read(sockfd, buffer, 255);
                    if (n < 0) {
                        perror("Error reading from socket");
                        return 6;
                    }
                    usleep(5);
                    if (strncmp("Done", buffer, 4) == 0) {
                        break;
                    }
                    printf("%s\n", buffer);
                }
                bzero(buffer, 256);
                fgets(buffer, 255, stdin);
                n = write(sockfd, buffer, strlen(buffer));
                if (n < 0) {
                    perror("Error writing to socket");
                    return 5;
                }
                if (strncmp("0", buffer, 1) != 0) {
                    n = read(sockfd, buffer, 255);
                    if (n < 0) {
                        perror("Error reading from socket");
                        return 6;
                    }
                    printf("%s", buffer);
                    bzero(buffer, 256);
                    fgets(buffer, 255, stdin);
                    int dobre = sendFile(buffer, sockfd);
                    printf("%d", dobre);
                }
                break;*/
            case 13: //vytvorit skupinovy chat
                printf("Zadajte nazov group chatu: ");
                bzero(buffer, 256);
                fgets(buffer, 255, stdin);
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
                if (buffer[0] == 0) {
                    printf("Chat uspesne vytvoreny");
                } else {
                    printf("Nepodarilo sa vytvorit groupu");
                }
                break;
            case 14: //pridat frienda do group chatu
                printf("Koho chcete pridat do groupchatu? \n");
                while (1 == 1) {
                    bzero(buffer, 256);
                    n = read(sockfd, buffer, 255);
                    if (n < 0) {
                        perror("Error reading from socket");
                        return 6;
                    }
                    if (strncmp("Done", buffer, 4) == 0) {
                        break;
                    }
                    printf("%s\n", buffer);
                    usleep(5);
                }
                bzero(buffer, 256);
                fgets(buffer, 255, stdin);
                n = write(sockfd, buffer, strlen(buffer));
                if (n < 0) {
                    perror("Error writing to socket");
                    return 5;
                }
                printf("A do ktorého? ");
                while(1 == 1) {
                    bzero(buffer, 256);
                    n = read(sockfd, buffer, 255);
                    if (n < 0) {
                        perror("Error reading from socket");
                        return 6;
                    }
                    printf("%s", buffer);
                    usleep(5);
                    if (strstr(buffer, "Done") !=NULL) {
                        break;
                    }
                }
                bzero(buffer, 256);
                fgets(buffer, 255, stdin);
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
                printf("%s", buffer);
                usleep(5);
                if (strncmp(buffer, "0", 1)==0) {
                    printf("User bol uspesne pridany");
                } else {
                    printf("Chyba pri vkladani usera, skontrolujte udaje");
                }
                break;
            case 15: //Napisat spravu skupine
                usleep(5);
                printf("A do ktoréj? \n");
                while(1 == 1) {
                    bzero(buffer, 256);
                    n = read(sockfd, buffer, 255);
                    if (n < 0) {
                        perror("Error reading from socket");
                        return 6;
                    }
                    if (strncmp("Done", buffer, 4) == 0) {
                        usleep(5);
                        break;
                    }
                    printf("%s", buffer);
                    printf("\n");
                    usleep(5);
                }
                bzero(buffer, 256);
                fgets(buffer, 255, stdin);
                n = write(sockfd, buffer, strlen(buffer));
                if (n < 0) {
                    perror("Error writing to socket");
                    return 5;
                }
                usleep(5);
                printf("Správa: ");
                bzero(buffer, 256);
                fgets(buffer, 255, stdin);
                n = write(sockfd, buffer, strlen(buffer));
                if (n < 0) {
                    perror("Error writing to socket");
                    return 5;
                }
                break;
            case 16: //pozriet spravy zo skupiny
                printf("A ktorého? ");
                while(1 == 1) {
                    bzero(buffer, 256);
                    n = read(sockfd, buffer, 255);
                    if (n < 0) {
                        perror("Error reading from socket");
                        return 6;
                    }
                    if (strncmp("Done", buffer, 4) == 0) {
                        break;
                    }
                    printf("%s", buffer);
                    printf("\n");
                    usleep(5);
                }
                usleep(5);
                bzero(buffer, 256);
                fgets(buffer, 255, stdin);
                n = write(sockfd, buffer, strlen(buffer));
                if (n < 0) {
                    perror("Error writing to socket");
                    return 5;
                }
                while(1 == 1) {
                    bzero(buffer, 256);
                    n = read(sockfd, buffer, 255);
                    if (n < 0) {
                        perror("Error reading from socket");
                        return 6;
                    }
                    if (strncmp("Done", buffer, 4) == 0) {
                        break;
                    }
                    printf("%s\n", buffer);
                    usleep(5);
                }
                break;
            default:
                break;
        }
    }
    }
    close(sockfd);

    return 0;
}