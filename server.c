#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include "server.h"
pthread_mutex_t mutex;

int login(char meno[], char heslo[]) {
    int found;
    found = 0;
    FILE *f;
    char fileMeno[20];
    char fileHeslo[20];
    meno[strcspn(meno, "\n")] = 0;
    heslo[strcspn(heslo, "\n")] = 0;
    pthread_mutex_lock(&mutex);
    f = fopen("/tmp/PosSemTest/login.txt", "r");
    while(!feof(f)) {
        bzero(fileMeno, 20);
        bzero(fileHeslo, 20);
        fscanf(f, "%s", fileMeno);
        fscanf(f, "%s", fileHeslo);
        if (strncmp(fileMeno, meno, strlen(meno)) == 0 && (strncmp(fileHeslo, heslo, strlen(heslo))) == 0) {
            found = 1;
            break;
        }
    }
    fclose(f);
    pthread_mutex_unlock(&mutex);
    return found;
}

void sendNotification(char meno[], char message[]) {
    FILE *f;
    pthread_mutex_lock(&mutex);
    f = fopen("/tmp/PosSemTest/contactNotifications.txt", "a");
    fprintf(f, "%s %s", meno, message);
    fclose(f);
    pthread_mutex_unlock(&mutex);
}

void readNotification(char meno[], int newsockfd) {
    FILE *f;
    char fileMeno[20];
    char message[256];
    int n;
    meno[strcspn(meno, "\n")] = 0;
    pthread_mutex_lock(&mutex);
    f = fopen("/tmp/PosSemTest/contactNotifications.txt", "r");
    while(!feof(f)) {
        bzero(fileMeno, 20);
        bzero(message, 256);
        fscanf(f, "%s", fileMeno);
        fgets(message, 256, f);
        if(strncmp(fileMeno, meno, strlen(meno))==0) {
            n = write(newsockfd, message, 255);
        }
    }
    fclose(f);
    pthread_mutex_unlock(&mutex);
    strcpy(message, "Done");
    n = write(newsockfd, message, 255);
}

int checkName(char meno[]) {
    int found;
    found = 0;
    FILE *f;
    char fileMeno[20];
    char fileHeslo[20];
    meno[strcspn(meno, "\n")] = 0;
    pthread_mutex_lock(&mutex);
    f = fopen("/tmp/PosSemTest/login.txt", "r");
    while(!feof(f)) {
        bzero(fileMeno, 20);
        bzero(fileHeslo, 20);
        fscanf(f, "%s", fileMeno);
        fscanf(f, "%s", fileHeslo);
        if (strncmp(fileMeno, meno, strlen(meno)) == 0) {
            found = 1;
            break;
        }
    }
    fclose(f);
    pthread_mutex_unlock(&mutex);
    return found;
}

void addUser(char meno[], char heslo[]) {
    FILE *f;
    pthread_mutex_lock(&mutex);
    f = fopen("/tmp/PosSemTest/login.txt", "a");
    fprintf(f, "%s %s", meno, heslo);
    fclose(f);
    pthread_mutex_unlock(&mutex);

    pthread_mutex_lock(&mutex);
    f = fopen("/tmp/PosSemTest/contacts.txt", "a");
    fprintf(f, "%s #", meno);
    fprintf(f, "\n");
    fclose(f);
    pthread_mutex_unlock(&mutex);
}

void removeUser(char meno[]) {
    char fileMeno[20];
    char fileHeslo[20];
    meno[strcspn(meno, "\n")] = 0;
    FILE *f, *n;
    pthread_mutex_lock(&mutex);
    f = fopen("/tmp/PosSemTest/login.txt", "r");
    n = fopen("/tmp/PosSemTest/loginnew.txt", "w+");
    while(!feof(f)) {
        bzero(fileMeno, 20);
        bzero(fileHeslo, 20);
        fscanf(f, "%s", fileMeno);
        if (strncmp(fileMeno, "0", 1) == 0) {
            break;
        }
        if (strncmp(fileMeno, meno, strlen(meno)) != 0) {
            fscanf(f, "%s", fileHeslo);
            fileHeslo[strcspn(fileHeslo, "\n")] = 0;
            fprintf(n, "%s %s", fileMeno, fileHeslo);
            fprintf(n,"\n");
        } else {
            fscanf(f, "%s", fileHeslo);
        }
    }
    fclose(f);
    fclose(n);
    remove("/tmp/PosSemTest/login.txt");
    rename("/tmp/PosSemTest/loginnew.txt", "/tmp/PosSemTest/login.txt");
    pthread_mutex_unlock(&mutex);

    char kontakty[1024];
    pthread_mutex_lock(&mutex);
    f = fopen("/tmp/PosSemTest/contacts.txt", "r");
    n = fopen("/tmp/PosSemTest/contactsnew.txt", "w+");
    while(!feof(f)) {
        bzero(fileMeno, 20);
        bzero(kontakty, 1024);
        fscanf(f, "%s", fileMeno);
        if (strncmp(fileMeno, "0", 1) == 0) {
            break;
        }
        if (strncmp(fileMeno, meno, strlen(meno)) != 0) {
            fgets(kontakty, 1024, f);
            fprintf(n, "%s %s", fileMeno, kontakty);
            fprintf(n,"\n");
        } else {
            fgets(kontakty, 1024, f);
        }
    }
    fclose(f);
    fclose(n);
    remove("/tmp/PosSemTest/contacts.txt");
    rename("/tmp/PosSemTest/contactsnew.txt", "/tmp/PosSemTest/contacts.txt");
    pthread_mutex_unlock(&mutex);
}

void getContacts(char meno[], int newsockfd) {
    FILE *f;
    int n;
    char fileMeno[20];
    char kontakty[1024];
    meno[strcspn(meno, "\n")] = 0;
    pthread_mutex_lock(&mutex);
    f = fopen("/tmp/PosSemTest/contacts.txt", "r");
    while(!feof(f)) {
        bzero(fileMeno, 20);
        fscanf(f, "%s", fileMeno);
        if (strncmp(fileMeno, meno, strlen(meno)) == 0) {
            while (1==1) {
                bzero(fileMeno, 20);
                fscanf(f, "%s", fileMeno);
                if (strncmp(fileMeno, "#", 1) != 0) {
                    if (strcspn(fileMeno, "%") == ((int)strlen(fileMeno)-1)) {
                        fileMeno[strcspn(fileMeno, "%")] = 0;
                        strcat(fileMeno, " je nepotvrdeny");
                    }
                    n = write(newsockfd, fileMeno, strlen(fileMeno) + 1);
                    usleep(5);
                } else {
                    strcpy(fileMeno, "Done");
                    n = write(newsockfd, fileMeno, strlen(fileMeno) + 1);
                    break;
                }
            }
            break;
        }
        fgets(kontakty, 1024, f);
    }
    fclose(f);
    pthread_mutex_unlock(&mutex);
}

int findContact(char meno[], char kontaktovany[]) {
    FILE *f;
    int found = 2;
    char fileMeno[20];
    char kontakty[1024];
    meno[strcspn(meno, "\n")] = 0;
    kontaktovany[strcspn(kontaktovany, "\n")] = 0;
    pthread_mutex_lock(&mutex);
    f = fopen("/tmp/PosSemTest/contacts.txt", "r");
    while(!feof(f)) {
        bzero(fileMeno, 20);
        bzero(kontakty, 1024);
        fscanf(f, "%s", fileMeno);
        if (strncmp(fileMeno, meno, strlen(meno)) == 0) {
            while (1==1) {
                bzero(fileMeno, 20);
                fscanf(f, "%s", fileMeno);
                if (strncmp(fileMeno, kontaktovany, strlen(kontaktovany)) == 0) {
                    found = 1;
                    break;
                } else if (strncmp(fileMeno, "#", 1) == 0){
                    found = 0;
                    break;
                }
            }
            break;
        }
        fgets(kontakty, 1024, f);
    }
    fclose(f);
    pthread_mutex_unlock(&mutex);
    return found;
}

void sendFriendRequest(char meno[], char kontaktovany[]) {
    FILE *f, *n;
    char kontakty[1024];
    char fileMeno[20];
    meno[strcspn(meno, "\n")] = 0;
    kontaktovany[strcspn(kontaktovany, "\n")] = 0;
    pthread_mutex_lock(&mutex);
    f = fopen("/tmp/PosSemTest/contacts.txt", "r");
    n = fopen("/tmp/PosSemTest/contactsnew.txt", "w+");
    while(!feof(f)) {
        bzero(fileMeno, 20);
        bzero(kontakty, 1024);
        fscanf(f, "%s", fileMeno);
        if (strncmp(fileMeno, "0", 1) == 0) {
            break;
        }
        if (strncmp(fileMeno, kontaktovany, strlen(meno)) == 0) {
            strcat(meno, "%");
            fgets(kontakty, 1024, f);
            fprintf(n, "%s %s %s", fileMeno, meno, kontakty);
        } else {
            fgets(kontakty, 1024, f);
            fprintf(n, "%s %s", fileMeno, kontakty);
        }
    }
    fclose(f);
    fclose(n);
    remove("/tmp/PosSemTest/contacts.txt");
    rename("/tmp/PosSemTest/contactsnew.txt", "/tmp/PosSemTest/contacts.txt");
    pthread_mutex_unlock(&mutex);
}

void getFriendRequests(char meno[], int newsockfd) {
    FILE *f;
    int n;
    char fileMeno[20];
    char kontakty[1024];
    meno[strcspn(meno, "\n")] = 0;
    pthread_mutex_lock(&mutex);
    f = fopen("/tmp/PosSemTest/contacts.txt", "r");
    while(!feof(f)) {
        bzero(fileMeno, 20);
        bzero(kontakty, 1024);
        fscanf(f, "%s", fileMeno);
        if (strncmp(fileMeno, meno, strlen(meno)) == 0) {
            while (1==1) {
                bzero(fileMeno, 20);
                fscanf(f, "%s", fileMeno);
                if (strncmp(fileMeno, "#", 1) != 0) {
                    if (strcspn(fileMeno, "%") == ((int)strlen(fileMeno)-1)) {
                        fileMeno[strcspn(fileMeno, "%")] = 0;
                        n = write(newsockfd, fileMeno, strlen(fileMeno) + 1);
                        usleep(5);
                    }
                } else {
                    strcpy(fileMeno, "Done");
                    n = write(newsockfd, fileMeno, strlen(fileMeno) + 1);
                    break;
                }
            }
            break;
        }
        fgets(kontakty, 1024, f);
    }
    fclose(f);
    pthread_mutex_unlock(&mutex);
}

void confirmFriendRequest(char meno[], char kontaktovany[]) {
    FILE *f, *n;
    char kontakty[1024];
    char kontaktProfile[20];
    char fileMeno[20];
    meno[strcspn(meno, "\n")] = 0;
    kontaktovany[strcspn(kontaktovany, "\n")] = 0;
    strcpy(kontaktProfile, kontaktovany);
    strcat(kontaktovany, "%");
    pthread_mutex_lock(&mutex);
    f = fopen("/tmp/PosSemTest/contacts.txt", "r");
    n = fopen("/tmp/PosSemTest/contactsnew.txt", "w+");
    while(!feof(f)) {
        bzero(fileMeno, 20);
        bzero(kontakty, 1024);
        fscanf(f, "%s", fileMeno);
        fprintf(n,"%s ", fileMeno);
        if (strncmp(fileMeno, "0", 1) == 0) {
            break;
        }
        if (strncmp(fileMeno, meno, strlen(meno)) == 0) {
            printf("Našiel som usera");
            while (1 == 1) {
                bzero(fileMeno, 20);
                fscanf(f, "%s", fileMeno);
                if (strncmp(fileMeno, "#", 1) != 0) {
                    if (strcspn(fileMeno, "%") == ((int) strlen(fileMeno) - 1)) {
                        if (strncmp(fileMeno, kontaktovany, strlen(kontaktovany)) == 0) {
                            fileMeno[strcspn(fileMeno, "%")] = 0;
                        }
                    }
                    fprintf(n, "%s ", fileMeno);
                } else {
                    fprintf(n, "%s ", fileMeno);
                    fprintf(n, "\n");
                    break;
                }
            }
        } else if(strncmp(fileMeno, kontaktProfile, strlen(kontaktProfile))==0) {
            fgets(kontakty, 1024, f);
            fprintf(n, "%s %s", meno, kontakty);
        } else {
            fgets(kontakty, 1024, f);
            fprintf(n, "%s", kontakty);
        }
    }
    fclose(f);
    fclose(n);
    remove("/tmp/PosSemTest/contacts.txt");
    rename("/tmp/PosSemTest/contactsnew.txt", "/tmp/PosSemTest/contacts.txt");
    pthread_mutex_unlock(&mutex);
}

void removeFriend(char meno[], char kontaktovany[]) {
    FILE *f, *n;
    char kontakty[1024];
    char fileMeno[20];
    meno[strcspn(meno, "\n")] = 0;
    kontaktovany[strcspn(kontaktovany, "\n")] = 0;
    pthread_mutex_lock(&mutex);
    f = fopen("/tmp/PosSemTest/contacts.txt", "r");
    n = fopen("/tmp/PosSemTest/contactsnew.txt", "w+");
    while(!feof(f)) {
        bzero(fileMeno, 20);
        bzero(kontakty, 1024);
        fscanf(f, "%s", fileMeno);
        fprintf(n,"%s ", fileMeno);
        if (strncmp(fileMeno, "0", 1) == 0) {
            break;
        }
        if (strncmp(fileMeno, meno, strlen(meno)) == 0) {
            while (1 == 1) {
                bzero(fileMeno, 20);
                fscanf(f, "%s", fileMeno);
                if (strncmp(fileMeno, "#", 1) != 0) {
                    if (strncmp(fileMeno, kontaktovany, strlen(kontaktovany)) != 0) {
                        fprintf(n, "%s ", fileMeno);
                    }
                } else {
                    fprintf(n, "%s ", fileMeno);
                    fprintf(n, "\n");
                    break;
                }
            }
        } else if(strncmp(fileMeno, kontaktovany, strlen(kontaktovany))==0) {
            while (1 == 1) {
                bzero(fileMeno, 20);
                fscanf(f, "%s", fileMeno);
                if (strncmp(fileMeno, "#", 1) != 0) {
                    if (strncmp(fileMeno, meno, strlen(meno) != 0)) {
                        fprintf(n, "%s ", fileMeno);
                    }
                } else {
                    fprintf(n, "%s ", fileMeno);
                    fprintf(n, "\n");
                    break;
                }
            }
        } else {
            fgets(kontakty, 1024, f);
            fprintf(n, "%s", kontakty);
            fprintf(n, "\n");
        }
    }
    fclose(f);
    fclose(n);
    remove("/tmp/PosSemTest/contacts.txt");
    rename("/tmp/PosSemTest/contactsnew.txt", "/tmp/PosSemTest/contacts.txt");
    pthread_mutex_unlock(&mutex);
    char message[256];
    bzero(message, 256);
    strcpy(message, "Pouzivatel ");
    strcat(message, meno);
    strcat(message, " si vas vymyzal z kontaktov");
    sendNotification(kontaktovany, message);
}

void sendMessage(char meno[], char kontaktovany[], char message[]) {
    kontaktovany[strcspn(kontaktovany, "\n")] = 0;
    meno[strcspn(meno, "\n")] = 0;
    FILE *f;
    pthread_mutex_lock(&mutex);
    f = fopen("/tmp/PosSemTest/messages.txt", "a");
    fprintf(f, "%s %s %s", kontaktovany, meno, message);
    fprintf(f, "\n");
    fclose(f);
    pthread_mutex_unlock(&mutex);
}

void readMessages(char meno[], char kontaktovany[], int newsockfd) {
    FILE *f;
    int n;
    char fileMeno[20];
    char message[256];
    meno[strcspn(meno, "\n")] = 0;
    kontaktovany[strcspn(kontaktovany, "\n")] = 0;
    pthread_mutex_lock(&mutex);
    f = fopen("/tmp/PosSemTest/messages.txt", "r");
    while(!feof(f)) {
        bzero(fileMeno, 20);
        bzero(message, 256);
        fscanf(f, "%s", fileMeno);
        if (strncmp(fileMeno, meno, strlen(meno)) == 0) {
            bzero(fileMeno, 20);
            fscanf(f, "%s", fileMeno);
            if (strncmp(fileMeno, kontaktovany, strlen(kontaktovany)) == 0) {
                fgets(message, 256, f);
                n = write(newsockfd, fileMeno, strlen(fileMeno) + 1);
                usleep(5);
                n = write(newsockfd, message, 255);
                usleep(5);
            } else {
                fgets(message, 256, f);
            }
        } else if (strncmp(fileMeno, kontaktovany, strlen(kontaktovany)) == 0) {
            bzero(fileMeno, 20);
            fscanf(f, "%s", fileMeno);
            if (strncmp(fileMeno, meno, strlen(meno)) == 0) {
                fgets(message, 256, f);
                n = write(newsockfd, fileMeno, strlen(fileMeno) + 1);
                usleep(5);
                n = write(newsockfd, message, 255);
                usleep(5);
            } else {
                fgets(message, 256, f);
            }
        } else {
            fscanf(f, "%s", fileMeno);
            fgets(message, 256, f);
        }
    }
    strcpy(fileMeno, "Done");
    n = write(newsockfd, fileMeno, strlen(fileMeno) + 1);
    fclose(f);
    pthread_mutex_unlock(&mutex);
}

int createGroupChat(char meno[], char groupName[]) {
    char members[1024];
    char groupID[20];
    groupName[strcspn(groupName, "\n")] = 0;
    meno[strcspn(meno, "\n")] = 0;
    FILE *f;
    pthread_mutex_lock(&mutex);
    f = fopen("/tmp/PosSemTest/groupChatContacts.txt", "r");
    while(!feof(f)) {
        bzero(members,1024);
        bzero(groupID, 20);
        fscanf(f, "%s", groupID);
        if(strncmp(groupID, groupName, strlen(groupName))==0) {
            return 2;
        }
        fgets(members, 1024, f);
    }
    fclose(f);
    f = fopen("/tmp/PosSemTest/groupChatContacts.txt", "a");
    fprintf(f, "%s %s #", groupName, meno);
    fclose(f);
    pthread_mutex_unlock(&mutex);
    return 0;
}

int addToGroupChat(char meno[], char pridany[], char groupName[]) {
    char members[1024];
    char member[20];
    char groupID[20];
    int found;
    groupName[strcspn(groupName, "\n")] = 0;
    meno[strcspn(meno, "\n")] = 0;
    pridany[strcspn(pridany, "\n")] = 0;
    FILE *f, *n;
    pthread_mutex_lock(&mutex);
    f = fopen("/tmp/PosSemTest/groupChatContacts.txt", "r");
    while(!feof(f)) {
        bzero(groupID, 20);
        bzero(members,1024);
        fscanf(f, "%s", groupID);
        if(strncmp(groupID, groupName, strlen(groupID))==0) {
            while(1 == 1) {
                bzero(member,20);
                fscanf(f, "%s", member);
                if(strncmp(member, pridany, strlen(member)) == 0) {
                    return 2;
                }
                if(strncmp(member, "#", 1)==0) {
                    break;
                }
            }
        } else {
            fgets(members, 1024, f);
        }
    }
    fclose(f);
    f = fopen("/tmp/PosSemTest/groupChatContacts.txt", "r");
    n = fopen("/tmp/PosSemTest/groupChatContactsnew.txt", "w+");
    while(!feof(f)) {
        bzero(groupID, 20);
        bzero(members,1024);
        fscanf(f, "%s", groupID);
        fprintf(n, "%s ", groupID);
        if(strncmp(groupID, groupName, strlen(groupID))==0) {
            fgets(members, 1024, f);
            fprintf(n, "%s %s", pridany, members);
        } else {
            fgets(members, 1024, f);
            fprintf(n, "%s", members);
        }
    }
    fclose(f);
    fclose(n);
    remove("/tmp/PosSemTest/groupChatContacts.txt");
    rename("/tmp/PosSemTest/groupChatContactsnew.txt", "/tmp/PosSemTest/groupChatContacts.txt");
    pthread_mutex_unlock(&mutex);
    return 0;
}

int checkGroup(char meno[], char groupName[]) {
    char member[20];
    char others[1024];
    char groupID[20];
    int found = 99;
    groupName[strcspn(groupName, "\n")] = 0;
    meno[strcspn(meno, "\n")] = 0;
    FILE *f;
    pthread_mutex_lock(&mutex);
    f = fopen("/tmp/PosSemTest/groupChatContacts.txt", "r");
    while(!feof(f)) {
        bzero(groupID,20);
        fscanf(f, "%s", groupID);
        if(strncmp(groupID, groupName, strlen(groupID))==0) {
            while(1==1) {

                bzero(member,20);
                fscanf(f, "%s", member);
                if(strncmp(member, meno, strlen(meno))==0){
                    found = 1;
                    fclose(f);
                    return found;
                }
                if(strncmp(member, "#", 1)==0) {
                    fclose(f);
                    return found;
                }
            }
        } else {
            bzero(others, 1024);
            fgets(others, 1024, f);
        }
    }
    fclose(f);
    pthread_mutex_unlock(&mutex);
    return found;
}

int sendGroupMessage(char meno[], char groupName[], char message[]) {
    if (checkGroup(meno, groupName)==1) {
        FILE *f;
        pthread_mutex_lock(&mutex);
        f = fopen("/tmp/PosSemTest/groupChatMessages.txt", "a");
        fprintf(f, "%s %s %s", groupName, meno, message);
        fclose(f);
        pthread_mutex_unlock(&mutex);
        return 0;
    } else {
        return 1;
    }
}

void readGroupMessages(char meno[], char groupName[], int newsockfd) {
    char message[256];
    char groupID[20];
    char fileMeno[20];
    int n;
    groupName[strcspn(groupName, "\n")] = 0;
    meno[strcspn(meno, "\n")] = 0;
    if (checkGroup(meno, groupName)==1) {
        FILE *f;
        f = fopen("/tmp/PosSemTest/groupChatMessages.txt", "r");
        while(!feof(f)) {
            bzero(message,256);
            bzero(groupID, 20);
            bzero(fileMeno, 20);
            fscanf(f, "%s ", groupID);
            if(strncmp(groupID, groupName, strlen(groupID))==0) {
                fscanf(f, "%s ", fileMeno);
                strcat(fileMeno, ": ");
                n = write(newsockfd, fileMeno, strlen(fileMeno));
                fgets(message, 256, f);
                n=write(newsockfd, message, strlen(message));
                usleep(5);
            } else {
                fscanf(f, "%s", fileMeno);
                fgets(message, 256, f);
            }
        }
        fclose(f);
    }
    bzero(message, 256);
    strcpy(message, "Done");
    n = write(newsockfd, message, strlen(message));
}

void getGroupchats(char meno[], int newsockfd) {
    char member[20];
    char groupID[20];
    int n;
    meno[strcspn(meno, "\n")] = 0;
    FILE *f;
    pthread_mutex_lock(&mutex);
    f = fopen("/tmp/PosSemTest/groupChatContacts.txt", "r");
    while(!feof(f)) {
        bzero(member,20);
        bzero(groupID, 20);
        fscanf(f, "%s", groupID);
        if (strncmp(groupID, "0", 1) != 0) {
            while (1==1) {
                bzero(member,20);
                fscanf(f, "%s", member);
                if (strncmp(member, meno, strlen(meno)) == 0) {
                    n = write(newsockfd, groupID, strlen(groupID));
                    usleep(5);
                }
                if (strncmp(member, "#", 1) == 0 || strlen(member) == 0) {
                    break;
                }
            }
        }
        if(strlen(groupID)==0) {
            break;
        }
    }
    fclose(f);
    pthread_mutex_unlock(&mutex);
    bzero(groupID, 20);
    strcpy(groupID, "Done");
    n = write(newsockfd, groupID, strlen(groupID));
}

void *userInteraction(void* arg) {
    int n;
    int newsockfd = *((int* ) arg);
    char buffer[256];
    char vyzva[50];
    bzero(vyzva,50);
    char search[20];
    bzero(search,20);
    bzero(buffer, 256);
    int logged = 0;
    int action = 0;
    char username[20];
    bzero(username, 20);
    char meno[20];
    char heslo[20];
    int on = 1;
    while(on==1) {
        //login/register
        while (logged == 0) {
            bzero(buffer, 256);
            n = read(newsockfd, buffer, 255);
            action = atoi(buffer);
            switch(action) {
                case 1:
                    bzero(vyzva, 50);
                    strcpy(vyzva, "Meno: ");
                    n = write(newsockfd, vyzva, strlen(vyzva) + 1);
                    bzero(buffer, 256);
                    n = read(newsockfd, buffer, 255);
                    bzero(meno, 20);
                    strcpy(meno, buffer);
                    strcpy(username, buffer);
                    bzero(buffer, 256);
                    bzero(vyzva, 50);
                    strcpy(vyzva, "Heslo: ");
                    n = write(newsockfd, vyzva, strlen(vyzva) + 1);
                    n = read(newsockfd, buffer, 255);
                    bzero(heslo, 20);
                    strcpy(heslo, buffer);
                    logged = login(meno, heslo);
                    if (logged == 0) {
                        bzero(vyzva, 50);
                        strcpy(vyzva, "Uzivatel nenajdeny: ");
                        n = write(newsockfd, vyzva, strlen(vyzva) + 1);
                    } else {
                        bzero(vyzva, 50);
                        strcpy(vyzva, "Vitaj ");
                        strcat(vyzva, meno);
                        n = write(newsockfd, vyzva, strlen(vyzva) + 1);
                    }
                    break;
                case 2:
                    bzero(vyzva, 50);
                    strcpy(vyzva, "Zadajte meno: ");
                    n = write(newsockfd, vyzva, strlen(vyzva) + 1);
                    bzero(buffer, 256);
                    n = read(newsockfd, buffer, 255);
                    bzero(meno, 20);
                    strcpy(meno, buffer);
                    strcpy(username, buffer);
                    bzero(buffer, 256);
                    bzero(vyzva, 50);
                    strcpy(vyzva, "Zadajte heslo: ");
                    n = write(newsockfd, vyzva, strlen(vyzva) + 1);
                    n = read(newsockfd, buffer, 255);
                    bzero(heslo, 20);
                    strcpy(heslo, buffer);
                    bzero(vyzva,50);
                    if (checkName(meno) == 0) {
                        addUser(meno, heslo);
                        logged = login(meno, heslo);
                        strcpy(vyzva, "Vitaj ");
                        strcat(vyzva, meno);
                        n = write(newsockfd, vyzva, strlen(vyzva) + 1);
                    } else {
                        strcpy(vyzva, "Uzivatel s takým meno už existuje :(");
                        n = write(newsockfd, vyzva, strlen(vyzva) + 1);
                    }
                    break;
                default:
                    break;
            }
        }

        while (logged == 1) {
            bzero(buffer, 256);
            n = read(newsockfd, buffer, 255);
            action = atoi(buffer);
            switch (action) {
                case 1://free writing
                    while (1 == 1) {
                        bzero(buffer, 256);
                        n = read(newsockfd, buffer, 255);
                        if (strncmp("end", buffer, 3) == 0) {
                            break;
                        }
                        const char *msg = "I got your message";
                        n = write(newsockfd, msg, strlen(msg) + 1);
                    }
                    break;
                case 2: //zobrazit kontakty
                    getContacts(username, newsockfd);
                    break;
                case 3: //poslat ziadost o priatelstvo
                    bzero(buffer, 256);
                    n = read(newsockfd, buffer, 255);
                    bzero(search, 20);
                    strcpy(search, buffer);
                    if (findContact(username, search) == 1) {
                        bzero(buffer, 256);
                        strcpy(buffer,"Uzivatel uz je v kontaktoch\n");
                        n = write(newsockfd, buffer, strlen(buffer));
                    } else {
                        sendFriendRequest(username, search);
                        bzero(buffer, 256);
                        strcpy(buffer,"Uzivatelovi bola poslana ziadost\n");
                        n = write(newsockfd, buffer, strlen(buffer));
                    }
                    break;
                case 4: //potvrdit ziadost o priatelstvo
                    getFriendRequests(username, newsockfd);
                    bzero(buffer, 256);
                    n = read(newsockfd, buffer, 255);
                    if(strncmp(buffer, "0", 1)!=0) {
                        confirmFriendRequest(username, buffer);
                    }
                    break;
                case 5: //vymazat z priatelov
                    getContacts(username, newsockfd);
                    bzero(buffer, 256);
                    n = read(newsockfd, buffer, 255);
                    if(strncmp(buffer, "0", 1)!=0) {
                        removeFriend(username, buffer);
                    }
                    break;
                case 6: //notifikacie
                    readNotification(username, newsockfd);
                    break;
                case 7: //precitat spravy
                    getContacts(username, newsockfd);
                    bzero(buffer, 256);
                    n = read(newsockfd, buffer, 255);
                    if(strncmp(buffer, "0", 1)!=0) {
                        readMessages(username, buffer, newsockfd);
                    }
                    break;
                case 8: //napisat uzivatelovi
                    getContacts(username, newsockfd);
                    bzero(buffer, 256);
                    n = read(newsockfd, buffer, 255);
                    if(strncmp(buffer, "0", 1)!=0) {
                        strcpy(search, buffer);
                        bzero(buffer, 256);
                        n = read(newsockfd, buffer, 255);
                        sendMessage(username, search, buffer);
                    }
                    break;
                case 9: //vytvorit skupinovy chat
                    bzero(buffer, 256);
                    n = read(newsockfd, buffer, 255);
                    n = createGroupChat(username, buffer);
                    buffer[0] = n;
                    n = write(newsockfd, buffer, strlen(buffer));
                    break;
                case 10: //pridat frienda do group chatu
                    getContacts(username, newsockfd);
                    bzero(buffer, 256);
                    n = read(newsockfd, buffer, 255);
                    strcpy(search, buffer);
                    getGroupchats(username, newsockfd);
                    bzero(buffer, 256);
                    n = read(newsockfd, buffer, 255);
                    n = addToGroupChat(username, search, buffer);
                    bzero(buffer, 255);
                    strcpy(buffer, "Error");
                    if (n == 0) {
                        bzero(buffer, 255);
                        buffer[0] = n + '0';
                    }
                    n = write(newsockfd, buffer, strlen(buffer));
                    break;
                case 11: //Napisat spravu skupine
                    getGroupchats(username, newsockfd);
                    usleep(5);
                    bzero(buffer, 256);
                    n = read(newsockfd, buffer, 255);
                    strcpy(search, buffer);
                    usleep(5);
                    bzero(buffer, 256);
                    n = read(newsockfd, buffer, 255);
                    sendGroupMessage(username, search, buffer);
                    break;
                case 12: //pozriet spravy zo skupiny
                    getGroupchats(username, newsockfd);
                    bzero(buffer, 256);
                    usleep(5);
                    n = read(newsockfd, buffer, 255);
                    readGroupMessages(username, buffer, newsockfd);
                    break;
                case 13: //odhlasit
                    logged = 0;
                    break;
                case 14: //vypnut klienta
                    logged = 0;
                    on = 0;
                    break;
                case 99: //zrusit ucet
                    removeUser(username);
                    logged = 0;
                    break;
                default:
                    break;
            }
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
    pthread_mutex_init(&mutex, NULL);

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
        pthread_create(&client, NULL, &userInteraction, &newsockfd);
    }
    close(sockfd);

    return 0;
}
