#include <dirent.h>
#include <sys/stat.h>

struct HNode{
	int num;
	char inst[20];
	struct HNode *next;
};
struct HNode *HEAD;
int HNode_cnt;
void help(void);
void freeAll(void);
void freeHistory(void);
void quit(void);
void addHistory(char *);
void printHistory(void);
void history(void);
void dir(void);
