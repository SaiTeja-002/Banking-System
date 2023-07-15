#ifndef STRUCTS_H
#define STRUCTS_H

typedef struct Credentials{
    int id;
    char password[50];
}Credentials;

typedef struct AdminCredentials{
    char username[50];
    char password[50];
}AdminCredentials;

typedef struct Account{
    int id;
    int balance;
}Account;

typedef struct User{
    int id;
    char password[50];
    int account_id;
    int account_type;
}User;

#endif
