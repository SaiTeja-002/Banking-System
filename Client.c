#include<stdio.h>
#include<stdlib.h>
#include<sys/socket.h>
#include<sys/types.h>
#include<unistd.h>
#include<string.h>
#include<netinet/in.h>
#include<arpa/inet.h>

#include "structs.h"


/*Function used by the Admin to view the account details of the users*/
void AccountHandler(long userSessionId, int sd)
{
    int option = 5;
    int accountDetails[4] = {0,0,0,0}; 
    
    printf("Writing sessionID\n");
    write(sd, &userSessionId, sizeof(userSessionId));
    printf("Writing option\n");
    write(sd, &option, sizeof(option)); 
    
    printf("Please enter the user id: ");
    int uid;
    scanf("%d", &uid);
    write(sd, &uid, sizeof(uid));

    printf("reading acc details\n");
    read(sd, accountDetails, sizeof(accountDetails));
    printf("Done reading\n");
    
    printf("UserID: %d\n", accountDetails[0]);
    printf("AccountNo: %d\n", accountDetails[1]);
    printf("accountType: %d\n", accountDetails[2]);
    printf("account balance: %d\n", accountDetails[3]);
}

/*Function used by the admin to change the password*/
void modifyAdminPasswordHandler(long userSessionId, int sd, char *username)
{
    char newPassword[50];
    int option = 4;

    printf("Input your new admin account password: ");
    scanf("%s", newPassword);

    write(sd, &userSessionId, sizeof(userSessionId));
    write(sd, &option, sizeof(option));
    write(sd, newPassword, sizeof(newPassword));
    write(sd, username, sizeof(username));

    int ret;
    read(sd, &ret, sizeof(ret));

    if (ret == 1)
        printf("admin password changed successfully\n");

    else
        printf("password change failed \n");
}

/*Function used by the admin to modify an account*/
void modifyAccountTypeHandler(long userSessionId, int sd)
{

    int accountNumber, accountType, option = 3, ret;

    printf("Input the accountNumber you want to modify: ");
    scanf("%d", &accountNumber);
    printf("New Account type:\n1. Single Account\n2. Joint Account\n");
    scanf("%d", &accountType);

    write(sd, &userSessionId, sizeof(userSessionId));
    write(sd, &option, sizeof(option));
    write(sd, &accountNumber, sizeof(accountNumber));
    write(sd, &accountType, sizeof(accountType));

    read(sd, &ret, sizeof(ret));

    if (ret == 1)
        printf("account no:%d's type successfully set\n", accountNumber);

    else
        printf("failed to change account type\n");
}

/*Function used by the admin to delete and account*/
void deleteAccountHandler(long userSessionId, int sd)
{

    int accountNumber;
    int option = 2;

    printf("Input the Account Number you want to delete: ");
    scanf("%d", &accountNumber);
    
    write(sd, &userSessionId, sizeof(userSessionId));
    write(sd, &option, sizeof(option));
    write(sd, &accountNumber, sizeof(accountNumber));

    int ret;
    read(sd, &ret, sizeof(ret));

    if (ret == 1)
        printf("account no: %d successfully delelted\n", accountNumber);

    else
        printf("Deletion failed\n");
}

/*Function used by admin to add a new account*/
void addAccountHandler(long userSessionId, int sd)
{
    int accountNumber, option = 1;

    printf("Input the accountNumber you want to add: ");
    scanf("%d", &accountNumber);
    
    write(sd, &userSessionId, sizeof(userSessionId));
    write(sd, &option, sizeof(option));
    write(sd, &accountNumber, sizeof(accountNumber));
   
    int ret;
    read(sd, &ret, sizeof(ret));

    if (ret == 1)
        printf("Account no: %d successfully added\n", accountNumber);
    
    else
        printf("Add account failed\n");
}

/*Prompts for the admin*/
int adminOptions(long userSessionId, int sd, char *username)
{
    write(1, "1. Add Account\n", sizeof("1. Add Account\n")); 
    write(1, "2. Delete Account\n", sizeof("1. Delete Account\n")); 
    write(1, "3. Modify Account Type\n", sizeof("3. Modify Account Type\n")); 
    write(1, "4. Password Change\n", sizeof("4. Password Change\n")); 
    write(1, "5. Account Details\n", sizeof("5. Account Details\n")); 
    write(1, "6. Exit\n", sizeof("6. Exit\n")); 

    int response;
    scanf("%d", &response);
    
    switch(response)
    {
        case 1: addAccountHandler(userSessionId, sd);
                break;

        case 2: deleteAccountHandler(userSessionId, sd);
                break;

        case 3: modifyAccountTypeHandler(userSessionId, sd);
                break;

        case 4: modifyAdminPasswordHandler(userSessionId, sd, username);
                break;

        case 5: AccountHandler(userSessionId, sd);
                break;

        case 6: ExitHandler(userSessionId, sd);
                break;
    }
    adminOptions(userSessionId, sd, username);
}

/*Function used to handle the admin login*/
void adminLogin(int sd)
{
    int type = 2;
    write(sd, &type, sizeof(type));

    AdminCredentials *cred = malloc(sizeof(AdminCredentials));

    printf("Please enter the Admin username: ");
    scanf("%s", cred->username);

    printf("Please enter the Admin password: ");
    scanf("%s", cred->password);

    printf("Writing the credentials to the server...\n");
    write(sd, cred, sizeof(AdminCredentials));  //Writing the admin account details to the server for validation
    
    long userSessionId;
    read(sd, &userSessionId, sizeof(userSessionId));

    if (userSessionId == -1)
        loginType(sd);

    else
        adminOptions(userSessionId, sd, cred->username);
}

/*Function used to handle the deposits of the user*/
void DepositHandler(long userSessionId, int sd)
{

    int deposit, success, option = 1;   //Option is used to indicate the type of operation

    write(1, "Input amount you would like to deposit: ", sizeof("Input amount you would like to deposit: "));
    scanf("%d", &deposit);
    
    write(sd, &option, sizeof(option));
    write(sd, &userSessionId, sizeof(userSessionId));
    write(sd, &deposit, sizeof(deposit));

    read(sd, &success, sizeof(success));

    if (success == 1)
        write(1, "successfully deposited money\n", sizeof("successfully deposited money\n"));
 
    else
        write(1, "Failed to deposit the money\n", sizeof("Failed to deposit the money\n"));
}

/*Function used to handle the withdraw events of the user*/
void WithdrawHandler(long userSessionId, int sd)
{

    int withdrawAmount, success, option = 2;

    write(1, "Enter the amount you would like to withdraw: ", sizeof("Enter the amount you would like to withdraw: "));
    scanf("%d", &withdrawAmount);
    
    write(sd, &option, sizeof(option));
    write(sd, &userSessionId, sizeof(userSessionId));
    write(sd, &withdrawAmount, sizeof(withdrawAmount));

    read(sd, &success, sizeof(success));

    if (success == 1)
        write(1, "Withdraw successful\n", sizeof("Withdraw successful\n"));
 
    else
        write(1, "Withdraw Failed\n", sizeof("Withdraw Failed\n"));
}

/*Fucntion used to handle the balance events of the user*/
void BalanceHandler(long userSessionId, int sd)
{

    int success, option = 3;
    
    write(sd, &option, sizeof(option));
    write(sd, &userSessionId, sizeof(userSessionId));

    read(sd, &success, sizeof(success));

    if (success > 0)
        printf("Current balance: %d\n", success);

    else
        printf("Unable to display balance\n");
}

/*Function used to change the password of the user*/
void PasswordHandler(long userSessionId, int sd)
{

    int option = 4, success;
    char newPassword[50];

    printf("Enter your new password: ");
    scanf("%s", newPassword);

    write(sd, &option, sizeof(option));
    write(sd, &userSessionId, sizeof(userSessionId));
    write(sd, newPassword, sizeof(newPassword));

    read(sd, &success, sizeof(success));

    if (success > 0)
        printf("Password Changed successfully\n");
 
    else
        printf("Unable to change password\n");
}

/*Function used to query the account details of the user*/
void viewAccountDetailsHandler(long userSessionId, int sd)
{

    int user_id, option = 5;
    printf("Please enter the user ID: ");
    scanf("%d", &user_id);

    write(sd, &option, sizeof(option));
    write(sd, &userSessionId, sizeof(userSessionId));

    // printf("Writing the user ID\n");
    // write(sd, &user_id, sizeof(user_id));
    
    int accountDetails[4] = {0, 0, 0, 0};   //Default values of the account is set to 0
    
    printf("Reading the account details...\n");
    read(sd, accountDetails, sizeof(accountDetails));   //Reading the account details from the server

    printf("UserID: %d\n", accountDetails[0]);
    printf("Account Number: %d\n", accountDetails[1]);
    printf("Account Type: %d\n", accountDetails[2]);
    printf("Account Balance: %d\n", accountDetails[3]);
}

/*Function used to handle the logout events*/
void ExitHandler(long userSessionId, int sd)
{
    close(sd);      //Closing socket descriptor
    printf("Successfully logged out!\n");

    exit(0);
}

/*User prompts*/
int userOptions(long userSessionId, int sd)
{
    write(1, "1. Deposit\n", sizeof("1. Deposit\n")); 
    write(1, "2. Withdraw\n", sizeof("1. Withdraw\n")); 
    write(1, "3. Balance Enquiry\n", sizeof("3. Balance Enquiry\n")); 
    write(1, "4. Password Change\n", sizeof("4. Password Change\n")); 
    write(1, "5. Account Details\n", sizeof("5. Account Details\n")); 
    write(1, "6. Logout\n", sizeof("6. Logout\n"));

    int choice;
    scanf("%d", &choice);

    switch(choice)
    {
        case 1: DepositHandler(userSessionId, sd);
                break;

        case 2: WithdrawHandler(userSessionId, sd);
                break;

        case 3: BalanceHandler(userSessionId, sd);
                break;

        case 4: PasswordHandler(userSessionId, sd);
                break;

        case 5: viewAccountDetailsHandler(userSessionId, sd);
                break;
        
        case 6: ExitHandler(userSessionId, sd);
                break;
    }

    userOptions(userSessionId, sd);
}

/* Taking credentials from the user */
void normalLogin(int sd)
{
    int type = 1;   //Used to identify that we are dealing with a normal user
    write(sd, &type, sizeof(type));
    
    Credentials *cred = malloc(sizeof(Credentials));

    write(1, "Please enter the User ID: ", sizeof("Please enter the User ID: "));
    scanf("%d", &cred->id);

    write(1, "Please enter your password: ", sizeof("Please enter your password: "));
    scanf("%s", cred->password);

    printf("%d\n%s\n", cred->id, cred->password);

    write(sd, cred, sizeof(Credentials));   //Passing the credentials to the server for validation

    printf("Done writing\n");
    
    long userSessionId;     //Getting the unique session ID
    read(sd, &userSessionId, sizeof(userSessionId));
    printf("%ld\n", userSessionId);

    /*If the credentials are invalid, then we go back to the login type page*/
    if (userSessionId == -1)
        loginType(sd);

    else    //If successfully logged in, we go to the operations page of the normal user
        userOptions(userSessionId, sd);
}

/* Used to identify the type of login the user wants */
void loginType(int sd)
{
    write(1, "Choose a login method\n1. Normal User\n2. Administrator\n", sizeof("Choose a login method\n1. Normal User\n2. Administrator\n")); 
    
    int choice;
    scanf("%d", &choice);

    switch(choice)
    {
        case 1 :    normalLogin(sd);
                    break;

        case 2 :    adminLogin(sd);
                    break;

        default:    write(1, "Invalid login type\n", sizeof("Invalid login type\n"));
                    break;
    }
}

int main()
{
    struct sockaddr_in server;
    int connSuccess;

    int sd = socket(AF_INET, SOCK_STREAM, 0);

    server.sin_family = AF_INET;
    server.sin_port = htons(6666);
    server.sin_addr.s_addr = INADDR_ANY;

    connSuccess = connect(sd, (struct sockaddr *)&server, sizeof(server));

    if(connSuccess < 0)
    {	
        printf("Connection failed\n");
        return -1;
    }

    loginType(sd);

    return 0;
}
