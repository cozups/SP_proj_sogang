#include "20172141.h"

void help() {
	/* proj 1 */
	printf("h[elp]\n");
	printf("d[ir]\n");
	printf("q[uit]\n");
	printf("hi[story]\n");
	printf("du[mp] [start, end]\n");
	printf("e[dit] address, value\n");
	printf("f[ill] start, end, value\n");
	printf("reset\n");
	printf("opcode mnemonic\n");
	printf("opcodelist\n");
	/* proj 2 */
	printf("assemble filename\n");
	printf("type filename\n");
	printf("symbol\n");
	/* proj 3 */
	printf("progaddr [address]\n");
	printf("loader [object filename1][object filename2][...]\n");
	printf("bp [address]\n");
	printf("run\n");
}
void freeAll() {
	// history, hashtable FREE
	freeHistory();
	freeHashtable();
	free_symtab(printHEAD);
}
void freeHistory() {
	// delete all nodes
	struct HNode* ptr = HEAD;
	struct HNode* temp;
	while (ptr != NULL) {
		temp = ptr;
		ptr = ptr->next;
		HEAD = ptr;
		HNode_cnt--;
		free(temp);
	}
	free(ptr);
}
void addHistory(char* his, int mode) {
	int i;
	/* mode 0 : opcode 외, mode 1 : opcode */
	if (mode == 0) {
		/* modify arguments to the correct form (ex: dump 004 -> dump 4) */
		for (i = 1; i < argc; i++) {
			int num = strtol(argv[i], NULL, 16);
			sprintf(argv[i], "%X", num);
		}
	}

	if (strcmp(argv[0], "loader") == 0 || strcmp(argv[0], "bp") == 0) {
		strcat(his, argv[0]);
		for (i = 1; i < argc; i++) {
			strcat(his, " ");
			strcat(his, argv[i]);
		}
		strcat(his, "\0");
	}
	else {
		/* modify input to the correct for for history (ex: dump AA   , BB -> dump AA, BB) */
		strcat(his, argv[0]);
		int idx = strlen(argv[0]);
		his[idx] = ' '; idx += 1;
		for (i = 1; i < argc - 1; i++) {
			strcat(his, argv[i]);
			idx += strlen(argv[i]);
			his[idx] = ','; idx += 1;
			his[idx] = ' '; idx += 1;
		}
		strcat(his, argv[i]);
		idx += strlen(argv[i]);
		his[idx] = '\0';
	}

	/* assign */
	struct HNode* newnode = malloc(sizeof(struct HNode));
	struct HNode* ptr = NULL;
	HNode_cnt++;
	newnode->num = HNode_cnt;
	newnode->next = NULL;
	strcpy(newnode->inst, his);

	/* Add nodes */
	if (HEAD == NULL) {
		HEAD = newnode;
	}
	else {
		for (ptr = HEAD; ptr->next != NULL; ptr = ptr->next);
		ptr->next = newnode;
	}
}

void printHistory() {
	// print history nodes
	struct HNode* ptr = NULL;
	if (HEAD == NULL) return;
	for (ptr = HEAD; ptr != NULL; ptr = ptr->next) {
		printf("%d\t %s\n", ptr->num, ptr->inst);
	}
}

void history() {
	if (HNode_cnt == 0) return;
	printHistory();
}

void dir() {
	DIR *stream = opendir(".");
	struct dirent *file = NULL;
	struct stat dir_info;
	if(stream == NULL){
		printf("opendir is failed\n");
		return;
	}

	file = readdir(stream);
	while(file != NULL){
		if(lstat(file->d_name, &dir_info) == -1){
			printf("stat error!\n");
			break;
		}
		else{
			if(S_ISDIR(dir_info.st_mode)){	// if directory
				if(strcmp(file->d_name, ".") == 0 || strcmp(file->d_name, "..") == 0){
					file = readdir(stream);
					continue;
				}
				printf("%s/\n", file->d_name);
			}
			else if(dir_info.st_mode & S_IXUSR || dir_info.st_mode & S_IXGRP || dir_info.st_mode & S_IXOTH){
				//if executable file
				printf("%s*\n", file->d_name);
			}
			//if not directory and executable file
			else printf("%s\n", file->d_name);
			file = readdir(stream);
		}
	}
	closedir(stream);
}
