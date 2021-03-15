#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "shell.h"
void help(){
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
}
void freeAll(){
	freeHistory();
}
void freeHistory(){
	struct HNode *ptr = HEAD;
	struct HNode *temp;
	while(ptr != NULL){
		temp = ptr;
		ptr = ptr->next;
		HEAD = ptr;
		HNode_cnt--;
		free(temp);
	}
	free(ptr);
}
void quit(){
	freeAll();
	exit(0);
}
void addHistory(char *cmd){
	struct HNode *newnode = malloc(sizeof(struct HNode));
	struct HNode *ptr = NULL;
	HNode_cnt++;
	newnode->num = HNode_cnt;
	strcpy(newnode->inst, cmd);
	newnode->next = NULL;
	
	if(HEAD == NULL){
		HEAD = newnode;
	}
	else{
		for(ptr = HEAD; ptr->next != NULL; ptr = ptr->next);
		ptr->next = newnode;
	}
}
void printHistory(){
	struct HNode *ptr= NULL;
	for(ptr = HEAD; ptr != NULL; ptr = ptr->next){
		printf("%d\t %s\n", ptr->num, ptr->inst);
	}
}
void history(){
	if(HNode_cnt == 0) return;
	printHistory();
}
void dir(){
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
				printf("%s/ ", file->d_name);
			}
			else if(dir_info.st_mode & S_IXUSR || dir_info.st_mode & S_IXGRP || dir_info.st_mode & S_IXOTH){
				//if executable file
				printf("%s* ", file->d_name);
			}
			else printf("%s ", file->d_name);
			file = readdir(stream);
		}
	}
	printf("\n");
	closedir(stream);
}
