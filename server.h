#ifndef SEMKA_SERVER_H
#define SEMKA_SERVER_H


        int login(char meno[], char heslo[]);
        void sendNotification(char meno[], char message[]);
        void readNotification(char meno[], int newsockfd);
        int checkName(char meno[]);
        void addUser(char meno[], char heslo[]);
        void removeUser(char meno[]);
        void getContacts(char meno[], int newsockfd);
        int findContact(char meno[], char kontaktovany[]);
        void sendFriendRequest(char meno[], char kontaktovany[]);
        void getFriendRequests(char meno[], int newsockfd);
        void confirmFriendRequest(char meno[], char kontaktovany[]);
        void removeFriend(char meno[], char kontaktovany[]);
        void sendMessage(char meno[], char kontaktovany[], char message[]);
        void readMessages(char meno[], char kontaktovany[], int newsockfd);
        int createGroupChat(char meno[], char groupName[]);
        int addToGroupChat(char meno[], char pridany[], char groupName[]);
        int checkGroup(char meno[], char groupName[]);
        int sendGroupMessage(char meno[], char groupName[], char message[]);
        void readGroupMessages(char meno[], char groupName[], int newsockfd);
        void getGroupchats(char meno[], int newsockfd);
        void *userInteraction(void* arg);
        int main(int argc, char *argv[]);
#endif //SEMKA_SERVER_H
