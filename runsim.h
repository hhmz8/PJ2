/*
Hanzhe Huang
10/4/2021
runsim.h
*/

void sigint(int sig);
void parent();
void child();
void deallocate();
struct shmseg* license();
void returnlicense(struct shmseg* shmp);
void initlicense();
void addtolicenses(int n);
void removelicenses(int n);
void docommand();