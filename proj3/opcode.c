#include "20172141.h"

int hash(char *str){
	// hash function : 각 ASCII code를 전부 더하여 20으로 나눈 나머지 반환
	int i, sum = 0;
	for(i=0; i<strlen(str); i++){
		sum += str[i];
	}

	return sum % 20;
}

void putHash(int idx, int num, char* inst, char *form) {
    // hashtable node assign
    struct HashNode* entry = malloc(sizeof(struct HashNode));
    struct HashNode* ptr = NULL;
    entry->num = num;
    strcpy(entry->inst, inst);
    strcpy(entry->format, form);
    entry->next = NULL;

    // add nodes to hashtable
    if (table[idx] == NULL) {
        table[idx] = entry;
    }
    else {
        for (ptr = table[idx]; ptr->next != NULL; ptr = ptr->next);
        ptr->next = entry;
    }
}

void setHashtable(){
	//file open
	FILE *fp = fopen("opcode.txt", "r");
	char name[10], form[5];
	int num, hashed;
	if(fp == NULL){
		printf("FILE OPEN ERROR!\n");
		return;
	}
  
	// read contents in file & add to hash table
	while(1){
		int res = fscanf(fp, "%x %s %s", &num, name, form);
		if(res == -1) break;
		hashed = hash(name);
		putHash(hashed, num, name, form);
	}

	fclose(fp);
}

void freeHashtable(){
	// delete all hashtable nodes
	struct HashNode *ptr = NULL;
	struct HashNode *temp = NULL;
	for(int i = 0; i < 20; i++){
		ptr = table[i];
		while(ptr != NULL){
		temp = ptr;
		ptr = ptr->next;
		table[i] = ptr;
		free(temp);
		}
	}
	free(ptr);
}

void OPList(){
	int i;
	struct HashNode *ptr = NULL;
	// print hashtable
	for(i=0; i<20; i++){
		printf("%d : ", i);
		ptr = table[i];
		if(ptr == NULL){
			printf("\n");
			continue;
		}
		for(ptr = table[i]; ptr->next != NULL; ptr = ptr->next){
			printf("[%s,%X] -> ", ptr->inst, ptr->num);
		}
		if(ptr != NULL)
			printf("[%s,%X]\n", ptr->inst, ptr->num);
	}
}

int OPmnemonic(char* target) {
    struct HashNode* ptr = NULL;
    int hashed = hash(target);
    // search target in hashtable
    for (ptr = table[hashed]; ptr != NULL; ptr = ptr->next) {
        if (strcmp(ptr->inst, target) == 0) {
            return ptr->num;
        }
    }

    // if not found -> error
    return -1;
}
