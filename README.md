# Banking-System
A basic Banking System implemented as part of the Operating Systems course

The project aims to develop a banking system that is user-friendly and multifunctional. The project has the following functionalities:
- All account transactional details and customer information are stored in files.
- Account holders have to pass through a login system to enter their accounts.
- The application should possess password-protected administrative access; thus preventing the whole management system from unauthorized access.
- Three types of login should be created: normal user, joint account user; administrator;
- Once you login as administrator, you should be able to add, delete, modify, search for a specific account details.
- Once the customer connect to the server, you should get login and password prompt.
After successful login, you should get menu for example:
  Do you want to:
  1. Deposit
  2. Withdraw
  3. Balance Enquiry
  4. Password Change
  5. View details
  6. Exit
- If you login as a joint account holder, proper file locking should be implemented. If you want to view the account details then read lock is to be set else if you want to withdraw or deposit then write lock should be used to protect the critical data section.
- Use socket programming – Server maintains the data base and service multiple clients concurrently. Client program can connect to the server and access their specific account details.
- Use system calls instead of Library functions wherever it is possible in the project:
Process Management, File Management, File Locking, Multithreading and Inter Process
Communication Mechanisms. 
