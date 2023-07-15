#include<stdio.h>
#include<stdlib.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<string.h>
#include<unistd.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<fcntl.h>

#include "structs.h"
#include "db.h"


void viewAccountDetails(int client)
{
    int user_id;

    read(client, &user_id, sizeof(user_id));
    viewDetails(client, user_id);
}

/*Function used to modify the Admin password*/
void modifyAdminPassword(int client)
{

    char newPassword[50];
    char username[50];

    read(client, newPassword, sizeof(newPassword));
    read(client, username, sizeof(username));
    
    int ret = dbModifyAdminPassword(newPassword, username);
    write(client, &ret, sizeof(ret)); 
}

/*Function used to handle the logout events*/
void clientLogout(int client)
{
    close(client);
    
    printf("Disconnected a client!\n");

    exit(0);
}

/*Function used to query the details of the user*/
void viewDetails(int client, int userID)
{
    // printf("In the view details function\n");
    int *details = malloc(sizeof(int)*4);
    
    // printf("accessing database\n");
    details = dbAccountDetails(userID);
    // printf("data accessed\n");

    //4 integers corresponding to 4 different filedsd are written to the client
    write(client, details, sizeof(int)*4);
    // printf("data written to the client\n");
} 

/*Funciton used to change the password of the user*/
void passwordChange(int client, int userID)
{
    // printf("In the password change funciton\n");

    char newPassword[50];
    int ret;

    read(client, newPassword, sizeof(newPassword)); //Getting the new password from the client

    // printf("Accessing the database\n");
    ret = dbChangePassword(newPassword, userID);
    // printf("database accessed\n");

    write(client, &ret, sizeof(ret));
    // printf("written to the client\n");
}

/*Funciton to check the balance of the user*/
void balance(int client, int userID)
{
    // printf("In the balance function\n");
    int balance = dbBalance(userID);    //Getting the current balance

    // printf("writing to the client\n");
    write(client, &balance, sizeof(balance));   //Writing it to the client

    // printf("Write successfull\n");
}

/*Funcion used to handle the database operations related to the withdraw function*/
void withdraw(int client, int userID)
{
    // printf("In the withdraw function\n");
    int withdrawAmount;

    read(client, &withdrawAmount, sizeof(withdrawAmount));
    // printf("Read the withdraw amount\n");

    // printf("accessing the database\n");
    int ret = dbWithdraw(withdrawAmount, userID);
    // printf("database accessed\n");

    write(client, &ret, sizeof(ret));    //Writing the success of the operation to the client
    // printf("Written to the client\n");
}

/*Function used to handle the database operations related to the deposit function*/
void deposit(int client, int userID)
{
    // printf("In the deposit fucntion\n");
    int depositAmount;

    // printf("Reading the amount\n");
    read(client, &depositAmount, sizeof(depositAmount));

    // printf("Accessing database\n");
    int ret = dbDeposit(depositAmount, userID);

    // printf("database accessed\n");
    write(client, &ret, sizeof(ret));   //Writing the success of the operation to the client
    // printf("deposited");
}

/*Funciton used to prompt the user at every step*/
void userOptions(int client, int userID, long clientSessionId)
{
    // printf("In the userOptions function\n");
    int type;
    long sessionIdRecv;
    
    read(client, &type, sizeof(type));
    // printf("typerecv - %d\n", type);
    read(client, &sessionIdRecv, sizeof(sessionIdRecv));
    // printf("sessionIdRecv - %ld\n", sessionIdRecv);

    if(sessionIdRecv == clientSessionId)
    {
        switch(type)
        {
            case 1: deposit(client, userID);
                    break;

            case 2: withdraw(client, userID);
                    break;

            case 3: balance(client, userID);
                    break;

            case 4: passwordChange(client, userID);
                    break;
                    
            case 5: viewDetails(client, userID);
                    break;

            case 6: clientLogout(client);
                    break;
        } 
    }

    userOptions(client, userID, clientSessionId);
}

long generateSessionID(int val)
{
    if (val == 1)
        return random();
    
    else
        return -1;
}

/*Function used to identify the normal user login credentials*/
void normalLogin(int client)
{
    // printf("In normal login function\n");
    Credentials *cred = malloc(sizeof(Credentials));

    // printf("reading credentials\n");
    read(client, cred, sizeof(Credentials));    //Reading the credentials of the user from the client

    // printf("validating credentials\n");
    int validity = validateLogin(cred);
    // printf("Generating session id\n");
    long clientSessionId = generateSessionID(validity); //Generating a session ID

    // printf("writing the session id\n");
    write(client, &clientSessionId, sizeof(clientSessionId));   //Writing the session ID of the user to the client
    // printf("done writing the session id\n");

    // printf("validity - %d\n", validity);

    /* If the credentials are valid, we proceed furthur */
    if(validity == 1)
        userOptions(client, cred->id,  clientSessionId);

    /*Else, we go back to the login type page*/
    else
        loginType(client);
}

/*Function used to modify the account*/
void modifyAccountType(int client)
{
    // printf("In the modifyaccounttype function\n");
    int accountNumber;
    int account_type;
     
    // printf("Reading the account number\n");
    read(client, &accountNumber, sizeof(accountNumber));
    // printf("Reading the account type\n");
    read(client, &account_type, sizeof(account_type));
    
    // printf("Accessing the database\n");
    int ret = dbModifyAccountType(accountNumber, account_type);

    write(client, &ret, sizeof(ret));
    // printf("Success has been written to the client\n");
}

/*Function used to delete the account*/
void deleteAccount(int client)
{
    // printf("In the deleteAccount function\n");
    int accountNumber;

    read(client, &accountNumber, sizeof(accountNumber));

    // printf("accessing the database\n");
    int ret = dbDeleteAccount(accountNumber);

    // printf("writing success to the client\n");
    write(client, &ret, sizeof(ret));
}

/*Function for the admin to add a new account*/
void addAccount(int client)
{
    // printf("In the addAccount function\n");
    int accountNumber;

    read(client, &accountNumber, sizeof(accountNumber));
    
    // printf("accessing the database\n");
    int ret = dbAddAccount(accountNumber);
    // printf("database accessed\n");

    write(client, &ret, sizeof(ret));
    // printf("Written to the client\n");
}

/*Prompts for the Admin*/
int adminOptions(int client, long clientSessionId)
{
    // printf("In the adminoptions function\n");
    int type;
    long sessionIdRecv;
    
    // printf("Reading the session id\n");
    read(client, &sessionIdRecv, sizeof(sessionIdRecv));
    // printf("Reading the type\n");
    read(client, &type, sizeof(type));

    if (sessionIdRecv == clientSessionId)
    {
        switch(type)
        {
            case 1: addAccount(client);
                    break;

            case 2: deleteAccount(client);
                    break;

            case 3: modifyAccountType(client);
                    break;

            case 4: modifyAdminPassword(client);
                    break;

            case 5: viewAccountDetails(client);
                    break;

            case 6: clientLogout(client);
                    break;

            default: printf("Invalid input choice\n");
                     break;
        } 
    }

    adminOptions(client, clientSessionId);
}

/*Function used to identify the normal user login credentials*/
void adminLogin(int client)
{
    // printf("In the admin login function\n");

    AdminCredentials *cred = malloc(sizeof(AdminCredentials));
    
    // printf("reading the credentials\n");
    read(client, cred, sizeof(AdminCredentials));
    
    // printf("accessing the database\n");
    int val = ValidateAdminLogin(cred);

    // printf("Generating the session ID\n");
    long clientSessionId = generateSessionID(val);  //Generating a session ID

    // printf("writing the session id\n");
    write(client, &clientSessionId, sizeof(clientSessionId));

    if (val == 1)
        adminOptions(client, clientSessionId);

    else
        loginType(client);
}

/* Used to identify the type of login the user wants */
void loginType(int client)
{   
    // printf("In loginType\n");

    int type;

    read(client, &type, sizeof(type));
    
    switch(type)
    {
        case 1 :    normalLogin(client);
                    break;

        case 2 :    adminLogin(client);
                    break;
    }
}


int main()
{
    int sd;

    struct sockaddr_in server;
    struct sockaddr_in client = {0};

    int clientSize = sizeof(client);
    int option = 1;

    sd = socket(AF_INET, SOCK_STREAM, 0);
    // perror("");
   
    //Forcing port reuse
    setsockopt(sd,SOL_SOCKET,SO_REUSEADDR|SO_REUSEPORT,&option,sizeof(option));
    
    // fcntl(sd, F_SETFL, fcntl(sd, F_GETFL, 0) | O_NONBLOCK);

    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons(6666);

    int bindSuccess = bind(sd, (struct sockaddr *)&server, sizeof(server));

    if(bindSuccess < 0)
    {
        write(1, "Binding Failed!!\n", sizeof("Binding Failed!!\n"));
        return -1;
    }

    printf("Server is ready\n");

    listen(sd, 10);

    int childpid;

    while(1)
    {
        int clientsd = accept(sd, (struct sockaddr *)&client, &clientSize);
        perror("");

        if(clientsd < 0)
        {
            write(1, "Connection Failed!\n", sizeof("Connection Failed!\n"));
            return -1;
        }

        if (!fork())
        {
            close(sd);
            loginType(clientsd);    
            return 1;
        }

        else
            close(clientsd);
    }

    return 0;
}
