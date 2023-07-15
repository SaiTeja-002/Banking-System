#ifndef DB_H
#define DB_H

#include "structs.h"

User getUser(int);
int getAccountID(int);
int validateLogin(Credentials *);
int ValidateAdminLogin(AdminCredentials *);
int dbDeposit(int, int);
int dbWithdraw(int, int);
int dbBalance(int);
int dbChangePassword(char *, int);
int *dbAccountDetails(int);
int dbDeleteAccount(int);
int dbModifyAccountType(int account_no, int account_type);
int dbModifyAdminPassword(char *newpass, char *username);
int dbAddAccount(int account_no);

#endif /* !DBOPERATIONS_H */
