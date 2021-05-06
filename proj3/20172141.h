/* 포함되는 파일 */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <dirent.h>
#include <sys/stat.h>
/* 정의되는 상수 */
#define MAX_ROW_SIZE 65536
#define MAX_COL_SIZE 16
#define MEMSIZE MAX_ROW_SIZE*MAX_COL_SIZE

int argc;
char argv[5][35];
char his[200];
int checkHex(char* str);
int tokenize(char*);
int CMD_classification(char*);
int CheckWrongInput(char* src);

/* shell part */
struct HNode {
	int num;
	char inst[100];
	struct HNode* next;
};
struct HNode* HEAD;
int HNode_cnt;
void help(void);
void freeAll(void);
void freeHistory(void);
void addHistory(char*, int);
void printHistory(void);
void history(void);
void dir(void);

/* memory part */
int v[3];
unsigned char mem[MEMSIZE];
int row, erow, addr;

int edit(int, unsigned int);
int fill(void);
int dump(void);

/* opcode part */
struct HashNode {
	int num;
	char inst[10];
	char format[5];
	struct HashNode* next;
};
struct HashNode* table[20];

void OPList(void);
int OPmnemonic(char*);
int hash(char*);
void putHash(int idx, int num, char* inst, char* form);
void freeHashtable(void);
void setHashtable(void);

/* assemble part */
struct symtab {
	char name[10];
	int addr;
	struct symtab* next;
};
struct symtab* symHEAD;
struct symtab* printHEAD;
int start_loc;
int end_loc;
int obj_start;
int obj_size;
char op[70];
int mod[100];
int mod_cnt;
char lstname[35];
char objname[35];
int typefile(char* name);
int search_symtab(char* target);
char* findFormat(char* target);
int isConstant(char* target);
void add_symtab(char* target, int loc);
int print_symtab();
int isRegister(char* target);
void objFile(FILE* fp, int opnum, int form, int loc);
int assemble(int pass, char* filename);
void free_symtab(struct symtab* target);

/* loader part */
int progaddr;
int csaddr;
int execaddr;
int total_length;
struct estab {
	char csec[20];
	char symname[20];
	int addr;
	int length;
	struct estab* next;
};
struct estab* esHEAD;
struct estab es_arr[50];
int ex[100];
int ex_idx;
int bp[100];
int bp_idx;
char CC;
typedef int reg_t;
reg_t A, X, L, PC, B, S, T;
int search_ESTAB(char* target, int mode);
int add_ESTAB(char* csecname, char* sym, int addr, int length);
void print_ESTAB();
int load(char* filename, int pass);
void modification(char* buf);
int breakpoint(char* par);
int run(void);
char* findmnemonic(int op);
void form2_operation(char* inst, reg_t r1, reg_t r2);
int getValfromMemory(int loc, int format);
void operation(char* inst, int addr, int ni);
reg_t findReg(reg_t r);
