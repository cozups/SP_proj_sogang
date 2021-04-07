#include "20172141.h"

int typefile(char *name){
  // type content of file
  
  FILE *fp = fopen(name, "r");
  DIR *dir = opendir(name);
  char buf;
  
  if(fp == NULL || dir){
    // if file doesn't exist or file is directory
    printf("FILE OPEN ERROR IN TYPE FUNCTION\n");
    return -1;
  }
  
  while(1){
		int res = fscanf(fp, "%c", &buf);
		if(res == -1) break;
		printf("%c", buf);
	}
  
  fclose(fp);
  return 0;
}

char* findFormat(char* target) {
    // find opcode's format & return
    
    struct HashNode* ptr = NULL;
    int hashed = hash(target);
    // search target in hashtable
    for (ptr = table[hashed]; ptr != NULL; ptr = ptr->next) {
        if (strcmp(ptr->inst, target) == 0) {
            return ptr->format;
        }
    }

    // if not found -> error
    return NULL;
}

int search_symtab(char* target) {
    // search SYMBOL in symbol table & return location
    
    struct symtab* ptr = symHEAD;
    if (symHEAD == NULL) return -1;
    for (ptr = symHEAD; ptr != NULL; ptr = ptr->next) {
        if (strcmp(target, ptr->name) == 0) {
            return ptr->addr;
        }
    }

    return -1;
}

int isConstant(char* target) {
    // check if it is number(constant)

    int i = 0;
    for (i = 0; i < strlen(target); i++) {
        if (target[i] >= '0' && target[i] <= '9')
            continue;
        else return 0;
    }

    return 1;
}

void add_symtab(char* target, int loc) {
    // add SYMBOL in symbol table in ascending order

    struct symtab* newnode = malloc(sizeof(struct symtab));
    struct symtab* ptr = NULL;
    struct symtab* prev = NULL;
    int found = 0;
    strcpy(newnode->name, target);
    newnode->addr = loc;
    newnode->next = NULL;

    if (symHEAD == NULL) {
        symHEAD = newnode;
    }
    else {
        for (ptr = symHEAD; ptr != NULL; ptr = ptr->next) {
            // find correct location & insert
            if (strcmp(ptr->name, newnode->name) < 0) {
                prev = ptr;
                continue;
            }
            else {
                if (prev == NULL) symHEAD = newnode;
                else prev->next = newnode;
                newnode->next = ptr;
                found = 1;
                break;
            }
            prev = ptr;
        }
        if(!found)
            prev->next = newnode;
    }
}

int print_symtab() {
    // print SYMBOL in symbol table

    struct symtab* ptr = NULL;
    if (printHEAD == NULL) {
      printf("Symbol table is not created.\n");
      return 0;
    }
    for (ptr = printHEAD; ptr->next != NULL; ptr = ptr->next) {
        printf("\t%s\t%04X\n", ptr->name, ptr->addr);
    }
    printf("\t%s\t%04X", ptr->name, ptr->addr);
    
    return 0;
}

int isRegister(char *target) {
    // check if it is register & return register number

    if (strcmp(target, "A") == 0) return 0x0000;
    if (strcmp(target, "X") == 0) return 0x0001;
    if (strcmp(target, "L") == 0) return 0x0002;
    if (strcmp(target, "PC") == 0) return 0x0008;
    if (strcmp(target, "SW") == 0) return 0x0009;
    if (strcmp(target, "B") == 0) return 0x0003;
    if (strcmp(target, "S") == 0) return 0x0004;
    if (strcmp(target, "T") == 0) return 0x0005;
    if (strcmp(target, "F") == 0) return 0x0006;

    return -1;
}

void objFile(FILE *fp, int opnum, int form, int loc) {
    // record .obj file ("T" parts)
    
    char temp[10];
    if (obj_size == 0) {
        // start new line
        fprintf(fp, "T%06X", loc);
    }
    obj_size += form;
    if (obj_size > 30) {
        // if out of range -> print & start new line
        fprintf(fp, "%02X", obj_size - form);
        fprintf(fp, "%s\n", op);
        fprintf(fp, "T%06X", loc);
        obj_size = form;
        memset(op, 0, strlen(op));
    }
    // if it is not out of range -> concatenate location
    if (form == 4) {
        sprintf(temp, "%08X", opnum);
        strcat(op, temp);
    }
    else if (form == 3) {
        sprintf(temp, "%06X", opnum);
        strcat(op, temp);
    }
    else if (form == 2) {
        sprintf(temp, "%04X", opnum);
        strcat(op, temp);
    }
    else if(form == 1){
        sprintf(temp, "%02X", opnum);
        strcat(op, temp);
    }
}

void free_symtab(struct symtab *target){
  // free SYMBOL TABLE: quit or error
  
  struct symtab *ptr = target;
	struct symtab *temp;
  while(ptr != NULL){
		temp = ptr;
		ptr = ptr->next;
		free(temp);
	}
	free(ptr);
	target = NULL;
}

int assemble(int pass, char *filename)
{
    // assemble .asm file -> make .lst file & .obj file
    
    char buf[100];
    char copy[100];
    char *ptr;
    char* form;
    char* label, * opcode, * operand1, * operand2; 
    int loc = 0, line = 0, eflag = 0, opnum = 0, base = 0, op_addr = 0, res = 0;
       
    /* check if file is .asm file */
    strcpy(copy, filename);
    ptr = strtok(copy, ".");
    ptr = strtok(NULL, ".");
    if(strcmp(ptr, "asm") != 0){
      printf("ERROR: input(%s) is not .asm file!\n", filename);
      return -1;
    }
    
    /* file open */
    FILE* fp = fopen(filename, "r");
    if(fp == NULL) {
      // file open error
      printf("%s file open ERROR!\n", filename);
      return -1;
    }
    
    /* make .lst .obj file name */
    strcpy(lstname, filename);
    strcpy(objname, filename);
    lstname[strlen(lstname) - 3] = 'l'; lstname[strlen(lstname) - 2] = 's'; lstname[strlen(lstname) - 1] = 't';
    objname[strlen(objname) - 3] = 'o'; objname[strlen(objname) - 2] = 'b'; objname[strlen(objname) - 1] = 'j';
    
    /* file write */
    FILE* ofp = fopen(lstname, "w");
    FILE* objfp = fopen(objname, "w");
      // file open error
    if(ofp == NULL) {
      printf("%s file open ERROR!\n", lstname);
      return -1;
    }
    if(objfp == NULL) {
      printf("%s file open ERROR!\n", objname);
      return -1;
    }
    
    
    /* pass 1 */
    // check start line
    fgets(buf, 100, fp);
    strcpy(copy, buf);
    label = strtok(copy, " ,\t\n");
    opcode = strtok(NULL, " ,\t\n");
    operand1 = strtok(NULL, " ,\t\n");
    operand2 = strtok(NULL, " ,\t\n");
    if (strcmp(opcode, "START") == 0) {
        loc = strtol(operand1, NULL, 16);
        start_loc = loc;
        obj_start = loc;
        ++line;
    }
    else {
        loc = 0;
        start_loc = loc;
        obj_start = loc;
        ++line;
    }
    if (pass == 2) {
        // write in .lst .obj file
        fprintf(ofp, "%d\t\t %04X\t\t %s\t %s\t %s\n", line * 5, loc, label, opcode, operand1);
        fprintf(objfp, "H%-6s%06X%06X\n", label, start_loc, end_loc);
    }

    /* pass 1 & 2 */
    while (fgets(buf, 100, fp) != NULL) {
        eflag = 0; opnum = 0;
        // check if comment
        if (buf[0] == '.') {
            ++line;
            if (pass == 2) {
              // write in .lst file
                fprintf(ofp, "%d\t\t %s", line * 5, buf);
            }
            continue;
        }
        // parse - no label
        if (strncmp(buf, "      ", 6) == 0) {
            strcpy(copy, buf);
            opcode = strtok(copy, " ,\t\n");
            operand1 = strtok(NULL, " ,\t\n");
            operand2 = strtok(NULL, " ,\t\n");
            strcpy(label, "      \0");
        }
        // parse - label exists
        else {
            strcpy(copy, buf);
            label = strtok(copy, " ,\t\n");
            // search SYMTAB for LABEL
            if (pass == 1) {
                if (search_symtab(label) != -1) {
                    //found -> error
                    ++line;
                    printf("Line %d in %s caused ERROR! - symbol already exists\n", line, filename);
                    return -1;
                }
                else {
                    add_symtab(label, loc);
                }
            }
            opcode = strtok(NULL, " ,\t\n");
            operand1 = strtok(NULL, " ,\t\n");
            operand2 = strtok(NULL, " ,\t\n");
        }
        line++;

        // pass 2 - print
        if (pass == 2 && strcmp(opcode, "BASE") != 0 && strcmp(opcode, "END") != 0) {
            if(operand1 == NULL)
                fprintf(ofp, "%d\t\t %04X\t\t %s\t %-6s\t", line * 5, loc, label, opcode);
            else if(operand2 != NULL)
                fprintf(ofp, "%d\t\t %04X\t\t %s\t %-6s\t %s, %-4s\t", line * 5, loc, label, opcode, operand1, operand2);
            else
                fprintf(ofp, "%d\t\t %04X\t\t %s\t %-6s\t %-10s\t", line * 5, loc, label, opcode, operand1);
        }
            
        // search OPTAB for OPCODE
        if(opcode[0] == '+'){
            // extended form
            strcpy(copy, opcode);
            ptr = strtok(copy, "+");
            strcpy(opcode, ptr);
            eflag = 1;
        }
        OPmnemonic(opcode);
        form = findFormat(opcode);
        
        if (strcmp(opcode, "BASE") == 0) {
            // set base
            base = search_symtab(operand1);
            if (pass == 2)  //just print
                fprintf(ofp, "%d\t\t\t\t\t %-6s\t %s\n", line * 5, opcode, operand1);
            continue;
        }
        
        if (form != NULL) {    // if opcode is in opcode.txt
            if (strcmp(form, "3/4") == 0) {
                if (eflag) loc += 4;
                else loc += 3;
            }
            else loc += atoi(form);
            // assemble object code
            if (pass == 2) {
                opnum = OPmnemonic(opcode);
                if (strcmp(form, "3/4") == 0) {
                    if (eflag) {
                        // form 4: use extended form
                        opnum = opnum << 24;
                        if (operand1 != NULL) {
                            if (operand1[0] == '#') {
                                // immediate addressing
                                opnum += 0x1 << 24;
                                strcpy(copy, operand1);
                                ptr = strtok(copy, "#");
                                strcpy(operand1, ptr);
                                op_addr = search_symtab(operand1);
                                if (op_addr != -1) {
                                    // #symbol
                                    opnum += op_addr;
                                    // record for relocation
                                    mod[mod_cnt] = loc - 4;
                                    mod_cnt++;
                                }
                                else {
                                    if (!isConstant(operand1)) {
                                        // not symbol & not constant
                                        printf("Line %d: This operand(%s) is not predefined symbol.\nAssemble Failed!\n", line, operand1);
                                        fclose(ofp);
                                        fclose(fp);
                                        return -1;
                                    }
                                    // #constant
                                    opnum += atoi(operand1);
                                }
                            }
                            else if (operand1[0] == '@') {
                                // indirect addressing
                                opnum += 0x2 << 24;
                                strcpy(copy, operand1);
                                ptr = strtok(copy, "@");
                                strcpy(operand1, ptr);
                                op_addr = search_symtab(operand1);
                                if (op_addr == -1) {
                                    // not symbol
                                    printf("Line %d: This operand(%s) is not predefined symbol.\nAssemble Failed!\n", line, operand1);
                                    fclose(ofp);
                                    fclose(fp);
                                    return -1;
                                }
                                opnum += op_addr;
                                // record for relocation
                                mod[mod_cnt] = loc - 4;
                                mod_cnt++;
                            }
                            else {
                                // simple addressing
                                opnum += 0x3 << 24;
                                op_addr = search_symtab(operand1);
                                if (op_addr == -1) {
                                    // not symbol
                                    printf("Line %d: This operand(%s) is not predefined symbol.\nAssemble Failed!\n", line, operand1);
                                    fclose(ofp);
                                    fclose(fp);
                                    return -1;
                                }
                                opnum += op_addr;
                                // record for relocation
                                mod[mod_cnt] = loc - 4;
                                mod_cnt++;
                            }
                        }
                        if (operand2 != NULL && isRegister(operand2) == 0x0001) {
                            // X register: add index bit
                            opnum += 0x8 << 20;
                        }
                        else {
                            // set E bit 1
                            opnum += 0x1 << 20;
                        }
                        // record in .obj file
                        objFile(objfp, opnum, 4, loc - 4);
                    }
                    else {
                        // form 3: choose PC relative or Base relative
                        opnum = opnum << 16;
                        if (operand1 != NULL) {
                            // except RSUB - no operand
                            if (operand1[0] == '#') {
                                // immediate addressing
                                opnum += 0x1 << 16;
                                strcpy(copy, operand1);
                                ptr = strtok(copy, "#");
                                strcpy(operand1, ptr);
                                op_addr = search_symtab(operand1);
                                if (op_addr != -1) {
                                    // #symbol
                                    if (abs(op_addr - loc) > 0x00000FFF) {
                                        if (abs(op_addr - base) > 0x00000FFF && eflag == 0) {
                                            // need extended form
                                            printf("Line %d: This opcode(%s) must use extended form. Include [+].\nAssemble Failed!\n", line, opcode);
                                        }
                                        // Base relative
                                        opnum += 0x4 << 12;
                                        opnum += (op_addr - base);
                                    }
                                    else {
                                        // PC relative
                                        opnum += 0x2 << 12;
                                        if (op_addr - loc < 0) {
                                            op_addr = (op_addr - loc) & 0x00000FFF;
                                            opnum += op_addr;
                                        }
                                        else opnum += (op_addr - loc);
                                    }
                                }
                                else {
                                    if (!isConstant(operand1)) {
                                        // not symbol & not constant
                                        printf("Line %d: This operand(%s) is not predefined symbol.\nAssemble Failed!\n", line, operand1);
                                        fclose(ofp);
                                        fclose(fp);
                                        return -1;
                                    }
                                    // #constant
                                    opnum += atoi(operand1);
                                }
                            }
                            else if (operand1[0] == '@') {
                                // indirect addressing
                                opnum += 0x2 << 16;
                                strcpy(copy, operand1);
                                ptr = strtok(copy, "@");
                                strcpy(operand1, ptr);
                                op_addr = search_symtab(operand1);
                                if (op_addr == -1) {
                                    // not symbol
                                    printf("Line %d: This operand(%s) is not predefined symbol.\nAssemble Failed!\n", line, operand1);
                                    fclose(ofp);
                                    fclose(fp);
                                    return -1;
                                }
                                if (abs(op_addr - loc) > 0x00000FFF) {
                                    if (abs(op_addr - base) > 0x00000FFF && eflag == 0) {
                                        // need extended form
                                        printf("Line %d: This opcode(%s) must use extended form. Include [+].\nAssemble Failed!\n", line, opcode);
                                    }
                                    // Base relative
                                    opnum += 0x4 << 12;
                                    opnum += (op_addr - base);
                                }
                                else {
                                    // PC relative
                                    opnum += 0x2 << 12;
                                    if (op_addr - loc < 0) {
                                        op_addr = (op_addr - loc) & 0x00000FFF;
                                        opnum += op_addr;
                                    }
                                    else opnum += (op_addr - loc);
                                }
                            }
                            else {
                                // simple addressing
                                opnum += 0x3 << 16;
                                op_addr = search_symtab(operand1);
                                if (op_addr == -1) {
                                    // not symbol
                                    printf("Line %d: This operand(%s) is not predefined symbol.\nAssemble Failed!\n", line, operand1);
                                    fclose(ofp);
                                    fclose(fp);
                                    return -1;
                                }
                                if (abs(op_addr - loc) > 0x00000FFF) {
                                    if (abs(op_addr - base) > 0x00000FFF && eflag == 0) {
                                        // need extended form
                                        printf("Line %d: This opcode(%s) must use extended form. Include [+].\nAssemble Failed!\n", line, opcode);
                                    }
                                    // Base relative
                                    opnum += 0x4 << 12;
                                    opnum += (op_addr - base);
                                }
                                else {
                                    // PC relative
                                    opnum += 0x2 << 12;
                                    if (op_addr - loc < 0) {
                                        op_addr = (op_addr - loc) & 0x00000FFF;
                                        opnum += op_addr;
                                    }
                                    else opnum += (op_addr - loc);
                                }
                            }
                            if (operand2 != NULL && isRegister(operand2) == 0x0001) {
                                // X register: add index bit
                                opnum += 0x8 << 12;
                            }
                        }
                        else if(strcmp(opcode, "RSUB") == 0){
                            // RSUB: no operand
                            opnum += 0x3 << 16;
                            fprintf(ofp, "\t\t");
                        }
                        else {
                            // except RSUB: must have operands
                            if (operand1 == NULL) {
                            // error: must have operand
                            printf("Line %d: This opcode(%s) does not have operands.\nAssemble Failed!\n", line, opcode);
                            fclose(ofp);
                            fclose(fp);
                            return -1;
                            }
                        }
                        // record in .obj file
                        objFile(objfp, opnum, 3, loc - 3);
                    }
                }
                else if(strcmp(form, "2") == 0){
                    // form 2: assemble register addresses
                    opnum = opnum << 8;
                    if (operand1 != NULL) {
                        op_addr = isRegister(operand1);
                        if (op_addr == -1) {
                            // not register operand
                            printf("Line %d: This operand(%s) is not register.\nAssemble Failed!\n", line, operand1);
                            fclose(ofp);
                            fclose(fp);
                            return -1;
                        }
                        opnum += (op_addr << 4);
                    }
                    if (operand2 != NULL) {
                        op_addr = isRegister(operand2);
                        if (op_addr == -1) {
                            // not register operand
                            printf("Line %d: This operand(%s) is not register.\nAssemble Failed!\n", line, operand2);
                            fclose(ofp);
                            fclose(fp);
                            return -1;
                        }
                        opnum += op_addr;
                    }
                    // record in .obj file
                    objFile(objfp, opnum, 2, loc - 2);
                }
                else {
                    // form 1: do nothing
                    if (operand1 != NULL && operand2 != NULL) {
                        // error: form 1 doesn't have operands
                        printf("Line %d: This opcode(%s) can't have operands.\nAssemble Failed!\n", line, opcode);
                        fclose(ofp);
                        fclose(fp);
                        return -1;
                    }
                    // record in .obj file
                    objFile(objfp, opnum, 1, loc - 1);
                }
            }
        }
        else if (strcmp(opcode, "WORD") == 0) {
            loc += 3; 
            if (pass == 2) {
                fprintf(ofp, "%06X", atoi(operand1));
                fprintf(ofp, "\n");
                continue;
            }
        }
        else if (strcmp(opcode, "RESW") == 0) {
            loc += 3 * atoi(operand1);
            // record in .obj file for line feed
            if (res == 0) {
                    fprintf(objfp, "%02X", obj_size);
                    fprintf(objfp, "%s\n", op);
                    obj_size = 0;
                    memset(op, 0, strlen(op));
                    res = 1;
            }
        }
        else if (strcmp(opcode, "RESB") == 0) {
            loc += atoi(operand1);
            // record in .obj file for line feed
            if (res == 0) {
                    fprintf(objfp, "%02X", obj_size);
                    fprintf(objfp, "%s\n", op);
                    obj_size = 0;
                    memset(op, 0, strlen(op));
                    res = 1;
            }
        }
        else if (strcmp(opcode, "BYTE") == 0) {
            strcpy(copy, operand1);
            ptr = strtok(copy, "'");
            if (strcmp(ptr, "C") == 0) {
                // if character
                ptr = strtok(NULL, "'");
                loc += strlen(ptr);
                if (pass == 2) {
                    // record in .lst file
                    fprintf(ofp, "\t\t");
                    for (int i = 0; i < strlen(ptr); i++) {
                        fprintf(ofp, "%02X", ptr[i]);
                    }
                    fprintf(ofp, "\n");
                    // record in .obj file
                    if (obj_size + strlen(ptr) > 30) {
                        fprintf(objfp, "%02X", obj_size);
                        fprintf(objfp, "%s\n", op);
                        fprintf(objfp, "T%06X", loc);
                        obj_size = strlen(ptr);
                        memset(op, 0, strlen(op));
                    }
                    else {
                        obj_size += strlen(ptr);
                        int len = strlen(ptr);
                        for (int i = 0; i < len; i++) {
                            sprintf(copy, "%02X", ptr[i]);
                            strcat(op, copy);
                        }
                    }
                    continue;
                }
            }
            else if (strcmp(ptr, "X") == 0) {
                // if hex
                ptr = strtok(NULL, "'");
                loc += (strlen(ptr) + 1) / 2;
                if (pass == 2) {
                    // record in .lst file
                    fprintf(ofp, "\t\t");
                    fprintf(ofp, "%s\n", ptr);
                    // record in .obj file
                    if (obj_size + strlen(ptr) > 30) {
                        fprintf(objfp, "%02X", obj_size);
                        fprintf(objfp, "%s\n", op);
                        fprintf(objfp, "T%06X", loc);
                        obj_size = strlen(ptr);
                        memset(op, 0, strlen(op));
                    }
                    else {
                        obj_size += strlen(ptr);
                        strcat(op, ptr);
                    }
                    continue;
                }
            }
        }
        else if (strcmp(opcode, "END") == 0) {
            if (pass == 2) {
                // record in .lst file
                fprintf(ofp, "%d\t\t\t\t\t %-6s  %s\n", line * 5, opcode, operand1);
                // record in .obj file
                fprintf(objfp, "%02X", obj_size);
                fprintf(objfp, "%s\n", op);
                // record modification code in. obj file for relocation
                for (int i = 0; i < mod_cnt; i++) {
                    fprintf(objfp, "M%06X05\n", mod[i] + 1);
                }
                fprintf(objfp, "E%06X\n", search_symtab(operand1));
            }
            break;
        }
        else {
            // wrong opcode -> error
            printf("Line %d caused ERROR! - invalid operation code: %s\n", line, opcode);
            return -1;
        }  
        if (pass == 2 && strcmp(opcode, "BASE") != 0 && strcmp(opcode, "END") != 0) {
            // record in .lst file
            if (strcmp(opcode, "RESW") == 0 || strcmp(opcode, "RESB") == 0) {
                fprintf(ofp, "\n");
            }
            else if (eflag) {
                fprintf(ofp, "\t\t%8X\n", opnum);
            }
            else
                fprintf(ofp, "\t\t%06X\n", opnum);
        }
    }
    end_loc = loc;
    if(pass == 2){
      // assemble success
      printf("[%s], [%s]\n", lstname, objname);
      printHEAD = symHEAD;
      symHEAD = NULL;
    }
    fclose(objfp);
    fclose(ofp);
    fclose(fp);

    return 0;
}
