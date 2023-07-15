#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>

#include "structs.h"

/*Returns the User structure corresponding to the given User ID*/
User getUser(int user_id)
{
    User user; 
    int seekAmount = user_id - 1; 

    int fd = open("./data/Users.dat", O_RDONLY,0744);

    static struct flock lock;

    /*Setting up a readlock*/
    lock.l_type = F_RDLCK;
    lock.l_start = seekAmount*sizeof(User);
    lock.l_whence = SEEK_SET;
    lock.l_len = sizeof(User);
    lock.l_pid = getpid();
    
    fcntl(fd, F_SETLK, &lock);

    lseek(fd, seekAmount*sizeof(user), SEEK_SET);
    read(fd, &user, sizeof(user));

    /*Unlocking the file*/
    lock.l_type = F_UNLCK;
    fcntl(fd, F_SETLK, &lock);
    close(fd);

    return user;
}

/*Function used to get the Account ID if the User ID is given*/
int getAccountID(int user_id)
{
    User user = getUser(user_id);
    return user.account_id;
}

/*Function used to deposit the money*/
int dbDeposit(int depositAmount, int user_id)
{
    Account acc;

    int account_id = getAccountID(user_id);
    int seekAmount = account_id - 1;

    int fd = open("./data/Accounts.dat", O_RDWR);

    static struct flock lock;

    /*Setting a write lock*/
    lock.l_type = F_WRLCK;
    lock.l_start = seekAmount*sizeof(acc);
    lock.l_whence = SEEK_SET;
    lock.l_len = sizeof(acc);
    lock.l_pid = getpid();

    fcntl(fd, F_SETLKW, &lock);
    
    lseek(fd, seekAmount*sizeof(acc), SEEK_SET);
    read(fd, &acc, sizeof(acc));

    acc.balance += depositAmount;

    lseek(fd, seekAmount*(sizeof(acc)), SEEK_SET);
    write(fd, &acc, sizeof(acc));
    
    lseek(fd, seekAmount*(sizeof(acc)), SEEK_SET);
    read(fd, &acc, sizeof(acc));
    
    /*Unlocing the file*/
    lock.l_type = F_UNLCK;
    fcntl(fd, F_SETLKW, &lock);
    close(fd);

    return 1;
}

/*Function used to withdraw money*/
int dbWithdraw(int withdrawAmount, int user_id)
{
    Account acc;

    int account_id = getAccountID(user_id);
    int seekAmount = account_id - 1;

    int fd = open("./data/Accounts.dat", O_RDWR);

    if(fd == -1)
      exit(1);

    static struct flock lock;

    /*Setting a write lock*/
    lock.l_type = F_WRLCK;
    lock.l_start = seekAmount*sizeof(acc);
    lock.l_whence = SEEK_SET;
    lock.l_len = sizeof(acc);
    lock.l_pid = getpid();

    fcntl(fd, F_SETLKW, &lock);

    lseek(fd,seekAmount*sizeof(acc),SEEK_SET);

    read(fd, &acc, sizeof(acc));
    
    if (withdrawAmount > acc.balance)
        return -1;

    acc.balance -= withdrawAmount;   //If the witdraw amount is valid, then it is deducted from the current balance

    lseek(fd, seekAmount*(sizeof(acc)), SEEK_SET);
    write(fd, &acc, sizeof(acc));
    
    lseek(fd, seekAmount*(sizeof(acc)), SEEK_SET);
    read(fd, &acc, sizeof(acc));
    
    /*Unlocking*/
    lock.l_type = F_UNLCK;
    fcntl(fd,F_SETLKW, &lock);
    close(fd);

    return 1;
}

/*Funciton to query the account details*/
int *dbAccountDetails(int user_id)
{
    static int account_details[4] = {0, 0, 0, 0};   //An array that stores the account details

    User user = getUser(user_id);
    int account_balance = dbBalance(user_id);
    
    account_details[0] = user_id;
    account_details[1] = user.account_id;
    account_details[2] = user.account_type;
    account_details[3] = account_balance;

    return account_details;
}

/*Function used to change the password*/
int dbChangePassword(char *newPassword, int user_id)
{
    User user; 
    int seekAmount = user_id - 1; 

    int fd = open("./data/Users.dat", O_RDWR);

    static struct flock lock;

    /*Setting a write lock*/
    lock.l_type = F_WRLCK;
    lock.l_start = seekAmount*sizeof(user);
    lock.l_whence = SEEK_SET;
    lock.l_len = sizeof(user);
    lock.l_pid = getpid();

    fcntl(fd, F_SETLKW, &lock);

    lseek(fd,seekAmount*sizeof(user),SEEK_SET);
    read(fd, &user, sizeof(user));
    
    strcpy(user.password, newPassword);
    lseek(fd, seekAmount*sizeof(user), SEEK_SET);
    write(fd, &user, sizeof(user));
    
    lseek(fd, seekAmount*sizeof(user), SEEK_SET);
    read(fd, &user, sizeof(user));
    
    /*Unlocking*/
    lock.l_type = F_UNLCK;
    fcntl(fd, F_SETLKW, &lock);
    close(fd);

    return 1;
}

/*Function used to query the balance*/
int dbBalance(int user_id)
{
    Account acc;

    int account_id = getAccountID(user_id);
    int seekAmount = account_id - 1;

    int fd = open("./data/Accounts.dat", O_RDWR);

    static struct flock lock;

    /*Setting a readlock*/
    lock.l_type = F_RDLCK;
    lock.l_start = seekAmount*sizeof(acc);
    lock.l_whence = SEEK_SET;
    lock.l_len = sizeof(acc);
    lock.l_pid = getpid();

    fcntl(fd, F_SETLKW, &lock);
    
    lseek(fd, seekAmount*sizeof(acc), SEEK_SET);
    read(fd, &acc, sizeof(acc));
    
    /*Unlocking*/
    lock.l_type = F_UNLCK;
    fcntl(fd,F_SETLKW, &lock);
    close(fd);

    return acc.balance;
}

/*Function used to validate the given credentials*/
int validateLogin(Credentials *cred)
{
    User user = getUser(cred->id); 

    int result;

    if (user.id == cred->id)
    {
        if(strcmp(user.password, cred->password) == 0)
            result = 1;

        else
            result = -1;
    }

    else
        result = -1;
    
    return result;
} 

/*Function used to validate the admin login credentials*/
int ValidateAdminLogin(AdminCredentials *cred)
{
    AdminCredentials adminCred; 

    int seekAmount = 0; 
    int fd = open("./data/Admins.dat", O_RDONLY,0744);

    static struct flock lock;
    
    //applying a readlock on the entire file to check usernames.
    lock.l_type = F_RDLCK;
    lock.l_start = 0; 
    lock.l_whence = SEEK_SET;
    lock.l_len = 0; 
    lock.l_pid = getpid();
    
    fcntl(fd, F_SETLK, &lock);

    int result = -1;

    lseek(fd, 0, SEEK_SET); 
    
    while(read(fd, &adminCred, sizeof(adminCred)))
    {
        if(strcmp(adminCred.username, cred->username) == 0 && strcmp(adminCred.password, cred->password) == 0)
            return 1;
    }

    /*Unlocking*/
    lock.l_type = F_UNLCK;

    fcntl(fd, F_SETLK, &lock);
    close(fd);

    return result;
} 

/*Function used to help the Admin to delete the user accounts*/
int dbDeleteAccount(int accountNumber)
{
    Account account;
    Account delete;
    delete.id = -1;
    delete.balance = -1;

    int fd = open("./data/Accounts.dat", O_RDWR,0744);
    
    static struct flock lock;
    
    //applying a Writelock on the entire file to check usernames.
    lock.l_type = F_WRLCK;
    lock.l_start = 0; 
    lock.l_whence = SEEK_SET;
    lock.l_len = 0; 
    lock.l_pid = getpid();
    
    int ret = fcntl(fd, F_SETLK, &lock);
    
    if (ret == -1)  return -1;

    int result = -1;
    lseek(fd, 0, SEEK_SET);

    while(read(fd, &account, sizeof(account)))
    {
        if (account.id == accountNumber)
        {
            lseek(fd,-sizeof(account), SEEK_CUR);
            write(fd, &delete, sizeof(delete));
            result = 0; 
        }
    }

    /*Unlocking*/
    lock.l_type = F_UNLCK;
    fcntl(fd, F_SETLK, &lock);
    close(fd);
    
    User user;

    fd = open("./data/Users.dat", O_RDWR,0744);

    if(fd == -1) 
      exit(1);

    static struct flock lock1;
    
    //applying a WriteLock on the entire file to check usernames.
    lock1.l_type = F_WRLCK;
    lock1.l_start = 0; 
    lock1.l_whence = SEEK_SET;
    lock1.l_len = 0; 
    lock1.l_pid = getpid();
    
    ret = fcntl(fd, F_SETLK, &lock1);
    
    lseek(fd, 0, SEEK_SET);

    while(read(fd, &user, sizeof(user)))
    {
        if (user.account_id == accountNumber)
        {
            user.account_id = -1;

            lseek(fd,-sizeof(user), SEEK_CUR);
            write(fd, &user, sizeof(user));

            result = 1; 
        }
    }

    /*Unlocking the file*/
    lock1.l_type = F_UNLCK;
    fcntl(fd, F_SETLK, &lock1);
    close(fd);

    return result;
}

/*Function used ot modify the account types*/
int dbModifyAccountType(int accountNumber, int accountType)
{
    User user;
    
    int fd = open("./data/Users.dat", O_RDWR,0744);
    int result = -1;
    
    static struct flock lock1;
    
    lock1.l_type = F_WRLCK;
    lock1.l_start = 0; 
    lock1.l_whence = SEEK_SET;
    lock1.l_len = 0; 
    lock1.l_pid = getpid();
    

    int ret = fcntl(fd, F_SETLK, &lock1);
    
    lseek(fd, 0, SEEK_SET);

    while(read(fd, &user, sizeof(user)))
    {
        if(user.account_id == accountNumber)
        {
            user.account_type = accountType;
            lseek(fd,-sizeof(user), SEEK_CUR);
            write(fd, &user, sizeof(user));
            result = 1; 
            break;
        }
    }

    /*Unlocking the file*/
    lock1.l_type = F_UNLCK;
    fcntl(fd, F_SETLK, &lock1);
    close(fd);

    return result;
}

/*Function used to modify the Admin password*/
int dbModifyAdminPassword(char *newPassword, char *username)
{
    AdminCredentials adminCred;

    int fd = open("./data/Admins.dat", O_RDWR,0744);
    
    static struct flock lock;
    
    //Applying a writelock on the entire file to check the usernames.
    lock.l_type = F_WRLCK;
    lock.l_start = 0; 
    lock.l_whence = SEEK_SET;
    lock.l_len = 0; 
    lock.l_pid = getpid();
    

    int ret = fcntl(fd, F_SETLK, &lock);
    
    lseek(fd,0,SEEK_SET);

    while(read(fd, &adminCred, sizeof(adminCred)))
    {
        if(strcmp(adminCred.username, username) == 0)
        {
            strcpy(adminCred.password, newPassword);
            lseek(fd,-sizeof(adminCred), SEEK_CUR);
            write(fd, &adminCred, sizeof(adminCred));
            lock.l_type = F_UNLCK;
            fcntl(fd, F_SETLK, &lock);
            close(fd);

            return 1;
        }
    }

    /*Unlocking the file*/
    lock.l_type = F_UNLCK;
    fcntl(fd, F_SETLK, &lock);
    close(fd);

    return -1;
}

/*Function used to add a new account to the database*/
int dbAddAccount(int accountNumber)
{
    
    char defaultPwd[50] = "DPSWD";

    int fd = open("./data/Accounts.dat", O_RDWR | O_APPEND);
    struct flock lock = {F_WRLCK, SEEK_SET, 0, 0, getpid()};
    fcntl(fd, F_SETLKW, &lock);

    Account tempAcc = {0};
    tempAcc.id = accountNumber;
    write(fd, &tempAcc, sizeof(Account));

    lock.l_type = F_UNLCK;fcntl(fd,F_SETLKW, &lock);
    close(fd);

    fd = open("./data/Users.dat", O_RDWR,0744);
    fcntl(fd, F_SETLKW, &lock);    
     
    int maxUserID = 1;

    while(1)
    {
        User temp;

        if(!read(fd,&temp,sizeof(User))) 
            break;

        maxUserID += 1;
    }

    User newUser;
    newUser.id = maxUserID; 
    newUser.account_id = accountNumber; 
    newUser.account_type = 0;

    for(int i = 0; i < sizeof(defaultPwd); i++)
        newUser.password[i] = defaultPwd[i];
    
    write(fd, &newUser, sizeof(newUser)); 
    
    lock.l_type = F_UNLCK;
    fcntl(fd, F_SETLK, &lock);

    close(fd);

    return 1;
}
