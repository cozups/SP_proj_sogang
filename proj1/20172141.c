#include "20172141.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

void tokenize(char *src, char *cmd){
	char temp[256];
	char *ptr;
	memset(temp, '\0', 256);
	strcpy(temp, src);
	
	ptr = strtok(temp, " ");
	strcpy(cmd, ptr);
}
int CMD_classification(char *cmd){
	if(strcmp(cmd, "help") == 0 || strcmp(cmd, "h") == 0){
		addHistory(cmd);
		return C_HELP;
	}
	if(strcmp(cmd, "dir") == 0 || strcmp(cmd, "d") == 0){
		addHistory(cmd);
		return C_DIR;
	}
	if(strcmp(cmd, "quit") == 0 || strcmp(cmd, "q") == 0){
		addHistory(cmd);
		return C_QUIT;
	}
	if(strcmp(cmd, "history") == 0 || strcmp(cmd, "hi") == 0){
		addHistory(cmd);
		return C_HISTORY;
	}
	if(strcmp(cmd, "dump") == 0 || strcmp(cmd, "du") == 0){
		return C_DUMP;
	}
	if(strcmp(cmd, "edit") == 0 || strcmp(cmd, "e") == 0){
		return C_EDIT;
	}
	if(strcmp(cmd, "fill") == 0 || strcmp(cmd, "f") == 0){
		return C_FILL;
	}
	if(strcmp(cmd, "reset") == 0){
		return C_RESET;
	}
	if(strcmp(cmd, "opcode") == 0){
		return C_OPMNEM;
	}
	if(strcmp(cmd, "opcodelist") == 0){
		return C_OPLIST;
	}

	return -1;
}

void CMD_execute(int flag){
	switch(flag){
		case C_HELP:
			help();
			break;
		case C_QUIT:
			quit();
			break;
		case C_HISTORY:
			history();
			break;
		case C_DIR:
			dir();
			break;
		default:
			break;
	}
}

int main(){
	char iSTR[256]; // input string
	char cmd[256]; // command
	char temp[256]; // temp string
	int c;
	HEAD = NULL; HNode_cnt = 0;
	while(1){
		//initialize
		memset(iSTR, '\0', 256);
		memset(cmd, '\0', 256);
		memset(temp, '\0', 256);

		printf("sicsim> ");
		scanf("%[^\n]", iSTR);
		if(iSTR[0] == '\0') {
			getchar();	
			continue;
		}
		strcpy(temp, iSTR);
		tokenize(temp, cmd);
		c = CMD_classification(cmd);
		CMD_execute(c);

		getchar();
	}


	return 0;
}

