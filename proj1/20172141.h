#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <dirent.h>
#include <sys/stat.h>
#define MAX_ROW_SIZE 65536
#define MAX_COL_SIZE 16
#define MEMSIZE MAX_ROW_SIZE*MAX_COL_SIZE

int argc;
char argv[5][10];
char his[200];
int checkHex(char *str);
int tokenize(char *);
int CMD_classification(char *);
int CheckWrongInput(char *src);

/* shell part */
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
void addHistory(char *, int);
void printHistory(void);
void history(void);
void dir(void);

/* memory part */
int v[3];
char mem[MEMSIZE];
int row, erow, addr;

int edit(void);
int fill(void);
int dump(void);

/* opcode part */
struct HashNode{
	int num;
	char inst[10];
	struct HashNode* next;
};
struct HashNode* table[20];

void OPList(void);
int OPmnemonic(char *);
int hash(char *);
void putHash(int idx, int num, char *inst);
void freeHashtable(void);
void setHashtable(void);
