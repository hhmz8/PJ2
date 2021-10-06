/*
Hanzhe Huang
10/4/2021
runsim.h
*/

void sigint(int sig);
void parent();
void child(int id, char* arg1, char* arg2, char* arg3);
void deallocate();
struct shmseg* license();
void getlicense(struct shmseg* shmp);
void returnlicense(struct shmseg* shmp);
void initlicense(struct shmseg* shmp);
void addtolicenses(struct shmseg* shmp, int n);
void removelicenses(struct shmseg* shmp, int n);
void docommand(char* arg1, char* arg2, char* arg3);