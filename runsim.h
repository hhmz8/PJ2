/*
Hanzhe Huang
10/4/2021
runsim.h
*/

void sigint(int sig);
void parent();
void child(int id);
void deallocate();
struct shmseg* license();
void returnlicense(struct shmseg* shmp);
void initlicense(struct shmseg* shmp);
void addtolicenses(struct shmseg* shmp, int n);
void removelicenses(struct shmseg* shmp, int n);
void docommand();