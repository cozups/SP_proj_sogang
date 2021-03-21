#include "20172141.h"

int CheckWrongInput(char *src){
	int i;
	char temp[100], *ptr;

	/* check if input is wrong */
	strcpy(temp, src);
	ptr = strtok(temp, " ");
	for(i = strlen(ptr); i< strlen(src); i++){
		/* check if input has wrong character */
		if(src[i] < 'a' && src[i] > 'z' && src[i] < 'A' && src[i] > 'Z'){
			if(src[i] != ' ' && src[i] != '\t' && src[i] != ',')
				return -1;
		}
	}

	/* ,,, -> 컴마가 연속되는 경우 error */
	for (i = 0; i < strlen(src) - 1; i++) {
		if (src[i] == ',' && src[i + 1] == ',') return -1;
	}
 
	return 0;
}

int checkHex(char *str) {
  	/* check if argument is hexadecimal */
	for (int i = 0; i < strlen(str); i++) {
		int res = isxdigit(str[i]);
		if (res == 0) return -1;
	}
	return 0;
}

int tokenize(char *src){
	char temp[256];
	char *ptr;
	int i;
	int check[256] = { 0, };
	memset(temp, '\0', 256);
	
	/* count number of argument & save */
	strcpy(temp, src);
	ptr = strtok(temp, ", \t");
	strcpy(argv[argc], ptr);
	argc++;
	while((ptr = strtok(NULL, ", \t"))){
	if(argc > 4) return -1;
		strcpy(argv[argc], ptr);
		argc++;
	}

	/* 잘못된 입력인지 확인 (컴마 위치)*/
	int j = 0;
	for (i = 0; i < strlen(src); i++) {
		if (strncmp(src + i, argv[j], strlen(argv[j])) == 0) {
      			// argument들이 위치한 곳에 숫자를 저장 -> argument가 아닌 부분에는 0이 저장될 것임(컴마, 스페이스, 탭)
			for (int k = i; k < i + strlen(argv[j]); k++) {
				check[k] = j + 1;
			}
			j++;
		}
	}
	int prev = -1, flag = 0;
	for (i = 0; i < strlen(src); i++) {
		if (check[i] != 0) {
			flag = 0;
			prev = check[i];
		}
		// 첫 번째 argument(command) 전에는 컴마가 없어야함-> 있다면 error
		if (check[i] == 0 && prev < 2 && src[i] == ',') return -1;
		// argumnet 사이 컴마 확인-> 컴마가 있어야함. 없다면 error (flag 1: 컴마 있음, 0: 컴마없음)
		if (prev >= 2) {    
			if (check[i] == 0 && src[i] == ',') flag = 1;
			if (i + 1 != strlen(src) && check[i + 1] > prev && flag == 0) return -1;
		}
		// 마지막 부분-> 마지막 argument 뒤에 컴마가 있다면 error
		if (prev == argc && check[i] == 0 && src[i] == ',') return -1;
	} 

	/* opcode 명령어가 아닌 경우에만 16진수 확인 */
	if(strcmp(argv[0], "opcode") != 0){
		/* check if argument is hexadecimal */
		for (i = 1; i < argc; i++) {
			if (checkHex(argv[i]) == -1) return -1;
		}
	}

	return 0;
}
int CMD_classification(char *cmd){
	// 주어진 명령어에 따른 함수 실행
	if(strcmp(cmd, "help") == 0 || strcmp(cmd, "h") == 0){
		if(argc > 1) return -1;
		help();
	}
	else if(strcmp(cmd, "dir") == 0 || strcmp(cmd, "d") == 0){
		if(argc > 1) return -1;
		dir();
	}
	else if(strcmp(cmd, "quit") == 0 || strcmp(cmd, "q") == 0){
		if(argc > 1) return -1;
		freeAll();
		return 1;
	}
	else if(strcmp(cmd, "history") == 0 || strcmp(cmd, "hi") == 0){
		if(argc > 1) return -1;
		addHistory(his, 0);
		history();
		return 0;
	}
	else if(strcmp(cmd, "dump") == 0 || strcmp(cmd, "du") == 0){
		if(dump() == -1) return -1;
	}
	else if(strcmp(cmd, "edit") == 0 || strcmp(cmd, "e") == 0){
		if(argc != 3) return -1;
		if(edit() == -1) return -1;
	}
	else if(strcmp(cmd, "fill") == 0 || strcmp(cmd, "f") == 0){
		if(argc != 4) return -1;
		if(fill() == -1) return -1;
	}
	else if(strcmp(cmd, "reset") == 0){
		if(argc > 1) return -1;
		memset(mem, 0, MEMSIZE);
	}
	else if(strcmp(cmd, "opcode") == 0){
		if(argc != 2) return -1;
		if(OPmnemonic(argv[1]) == -1) return -1;
		addHistory(his, 1);
		return 0;
	}
	else if(strcmp(cmd, "opcodelist") == 0){
		if(argc > 1) return -1;
		OPList();
	}
	else return -1;

	addHistory(his, 0);
	return 0;
}

int main(){
	char iSTR[256]; // input string
	char temp[256]; // temp string
 
	HEAD = NULL; HNode_cnt = 0;
	setHashtable();

	while(1){
		/* initialize */
		memset(iSTR, '\0', 256);
		memset(temp, '\0', 256);
		memset(his, '\0', 200);
		memset(argv, '\0', 50);
		memset(v, 0, 3*sizeof(int));
		argc = 0;

		/* input */
		printf("sicsim> ");
		scanf("%[^\n]", iSTR);
		if(iSTR[0] == '\0') {
			getchar();	
			continue;
		}

		/* check if input is wrong */
		int wrong = CheckWrongInput(iSTR);\
		if(wrong == -1){
			printf("YOU ENTERED WRONG INPUT.\n");
			getchar();
			continue;
		}
		
		/* tokenize input */
		strcpy(temp, iSTR);
		wrong = tokenize(temp);\
		if(wrong == -1){
			printf("YOU ENTERED WRONG INPUT.\n");
			getchar();
			continue;
		}

		/* cmd execute */
		int flag = CMD_classification(argv[0]);\
		if(flag == -1){
			printf("YOU ENTERED WRONG INPUT.\n");
			getchar();
			continue;	
		}
		else if(flag == 1){
			return 0;
		}

		getchar();
	}


	return 0;
}

