
#ifndef SEMKA_CLIENT_H
#define SEMKA_CLIENT_H

class client {
public:
    char* code(char message[]);
    char* decode(char message[]);
    int main(int argc, char *argv[]);
};

#endif //SEMKA_CLIENT_H
