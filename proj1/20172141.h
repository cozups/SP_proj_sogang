#include "shell.h"

enum CMD_NUM{
	C_HELP = 0, C_DIR, C_QUIT, C_HISTORY, C_DUMP, C_EDIT, C_FILL, C_RESET, C_OPMNEM, C_OPLIST
};


void tokenize(char *, char *);
int CMD_classification(char *);
void CMD_execute(int);
