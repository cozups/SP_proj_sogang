#include "20172141.h"

int search_ESTAB(char* target, int mode) {
    // search EXTERNAL SYMBOL in ESTAB

    struct estab* ptr = esHEAD;
    if (esHEAD == NULL) return -1;
    for (ptr = esHEAD; ptr != NULL; ptr = ptr->next) {
        if (mode == 1) {
            // find CONTROL SECTION NAME
            if (strcmp(ptr->csec, target) == 0) return ptr->addr;
        }
        else {
            // find EXTERNAL SYMBOL NAME
            if (strcmp(ptr->symname, target) == 0) return ptr->addr;
        }
    }

    return -1;
}

int add_ESTAB(char* csecname, char* sym, int addr, int length) {
    // insert EXTERNAL SYMBOL NODE to ESTAB

    /* assign & initialize */
    struct estab* newnode = malloc(sizeof(struct estab));
    struct estab* ptr = NULL;
    strcpy(newnode->csec, csecname);
    strcpy(newnode->symname, sym);
    newnode->addr = addr;
    newnode->length = length;
    newnode->next = NULL;

    /* insert node */
    if (esHEAD == NULL) {
        esHEAD = newnode;
    }
    else {
        for (ptr = esHEAD; ptr->next != NULL; ptr = ptr->next);
        ptr->next = newnode;
    }

    return 1;
}

void print_ESTAB() {
    // print history nodes
    struct estab* ptr = NULL;
    if (esHEAD == NULL) return;
    printf("CONTROL SYMBOL ADDRESS LENGTH\n");
    printf("SECTION NAME\n");
    printf("-----------------------------------\n");
    for (ptr = esHEAD; ptr != NULL; ptr = ptr->next) {
        if (ptr->length < 0) {
	// external symbol
            printf("%6s %6s   %04X       \n", ptr->csec, ptr->symname, ptr->addr);
        }
        else {
	// control section
            printf("%6s %6s   %04X   %04X\n", ptr->csec, ptr->symname, ptr->addr, ptr->length);
        }
    }
    printf("-----------------------------------\n");
    printf("\ttotal length:  %04X\n", total_length);
}

int load(char* filename, int pass) {
    FILE* fp = fopen(filename, "r");
    char buf[100];
    char copy[100], refer[3];
    char* ptr;
    char csname[20], sym[20], addr[10], len[10], value[6];
    int address, length, start, temp, loc = 0, ref;

    if (fp == NULL) {
        printf("File open error: file doesn't exist in directory\n");
        return -1;
    }

    while (fgets(buf, 100, fp) != NULL) {
        if (pass == 1) {
            if (buf[0] == 'H') {  // HEADER RECORD
                strncpy(csname, buf + 1, 6);
                csname[6] = '\0';
                ptr = strtok(csname, " \0\n");
                strcpy(csname, ptr);
                if (search_ESTAB(csname, 1) != -1) {
                    // external symbol exists in ESTAB -> error
                    printf("Error: CONTROL SECTION NAME exists in ESTAB\n");
                    return -1;
                }
                else {
                    // insert CONTROL SECTION to ESTAB
                    strncpy(addr, buf + 7, 6);
                    addr[6] = '\0';
                    strncpy(len, buf + 13, 6);
                    len[6] = '\0';
                    address = strtol(addr, NULL, 16);
                    length = strtol(len, NULL, 16);
                    add_ESTAB(csname, " ", csaddr + address, length);
                }
            }
            else if (buf[0] == 'D') {   // DEFINE RECORD
                strcpy(copy, buf + 1);
                while (copy[0] != '\0' && copy[0] != '\n') {
                    // while symbol exists in DEFINE RECORD
                    strncpy(sym, copy, 6);
                    sym[6] = '\0';
                    ptr = strtok(sym, " \0\n");
                    strcpy(sym, ptr);
                    strncpy(addr, copy + 6, 6);
                    addr[6] = '\0';
                    if (search_ESTAB(sym, 2) != -1) {
                        // external symbol exists in ESTAB -> error
                        printf("Error: External symbol exists in ESTAB\n");
                        return -1;
                    }
                    else {
                        // insert external symbol to ESTAB
                        address = strtol(addr, NULL, 16);
                        add_ESTAB(" ", sym, csaddr + address, -1);
                    }
                    strcpy(copy, copy + 12);
                }
            }
            else if (buf[0] == 'E') {   // END RECORD
                csaddr = csaddr + length;
                total_length += length;
            }
        }
        else {
            execaddr = progaddr;
            if (buf[0] == 'H') {  // HEADER RECORD
                strncpy(csname, buf + 1, 6);
                csname[6] = '\0';
                ptr = strtok(csname, " \0\n");
                strcpy(csname, ptr);
                strncpy(addr, buf + 7, 6);
                addr[6] = '\0';
                strncpy(len, buf + 13, 6);
                len[6] = '\0';
                address = strtol(addr, NULL, 16);
                length = strtol(len, NULL, 16);
                csaddr += address;
                strcpy(es_arr[1].symname, csname);
                es_arr[1].addr = csaddr + address;
            }
            else if (buf[0] == 'R') {	// REFER RECORD: create reference number array
                strcpy(copy, buf + 1);
                while (copy[0] != '\0' && copy[0] != '\n') {
                    strncpy(refer, copy, 2);
                    refer[2] = '\0';
                    ref = strtol(refer, NULL, 16);
                    strcpy(copy, copy + 2);
                    strncpy(sym, copy, 6);
                    sym[6] = '\0';
                    ptr = strtok(sym, " \n\0");
                    strcpy(sym, ptr);
                    strcpy(es_arr[ref].symname, sym);
                    es_arr[ref].addr = search_ESTAB(sym, 2);
                    strcpy(copy, copy + 6);
                }
            }
            else if (buf[0] == 'T') {   // TEXT RECORD
                strcpy(copy, buf + 1);
                strncpy(addr, copy, 6);
                addr[6] = '\0';
                start = strtol(addr, NULL, 16);
                strcpy(copy, copy + 6);
                strncpy(len, copy, 2);
                len[2] = '\0';
                strcpy(copy, copy + 2);
                for (int i = 0; copy[i] != '\n' && copy[i] != '\0'; i += 2, loc++) {
                    strncpy(value, copy + i, 2);
                    value[2] = '\0';
                    temp = strtol(value, NULL, 16);
                    edit(csaddr + start + loc, temp);
                }
                loc = 0;
            }
            else if (buf[0] == 'M') {   // MODIFICATION RECORD
                modification(buf);
            }
            else if (buf[0] == 'E') {   // END RECORD
                strncpy(copy, buf + 1, 6);
                copy[6] = '\0';
                address = strtol(copy, NULL, 16);
                execaddr = csaddr + address;
                csaddr += length;
            }
        }
    }
    fclose(fp);


    return 0;
}

void modification(char* buf) {
    char temp[10];
    int loc, halfbyte, val, ref, save;
    char flag;

    /* get modification location */
    strncpy(temp, buf + 1, 6);
    temp[6] = '\0';
    loc = strtol(temp, NULL, 16);
    loc += csaddr;

    /* get halfbyte information */
    strncpy(temp, buf + 7, 2);
    temp[2] = '\0';
    halfbyte = strtol(temp, NULL, 16);
    if (halfbyte == 5) {
        // save location of extended form.
        ex[ex_idx] = loc - 1;
        ex_idx++;
    }

    /* + or - */
    flag = buf[9];

    /* get reference number */
    strncpy(temp, buf + 10, 2);
    temp[2] = '\0';
    ref = strtol(temp, NULL, 16);

    /* get value in the memory*/
    val = mem[loc];
    val = val << 8;
    val += mem[loc + 1];
    val = val << 8;
    val += mem[loc + 2];
    if (halfbyte == 6 && ((val & 0x00800000) == 0x00800000)) {
        val += 0xFF000000;
    }

    if (flag == '+') {
        // plus
        val += es_arr[ref].addr;
    }
    else {
        // minus
        val -= es_arr[ref].addr;
    }

    /* modify value in the memory */
    save = val;
    val = val >> 16;
    edit(loc, val);
    val = save;
    val = val >> 8;
    edit(loc + 1, val);
    val = save;
    val = val & 0x000000FF;
    edit(loc + 2, val);
}

int breakpoint(char* par) {
    int loc;
    if (strcmp(par, "clear") == 0) {	// bp clear
        bp_idx = 0;
        memset(bp, 0, sizeof(int) * 100);
        printf("\t\t[ok] clear all breakpoints\n");
        return 0;
    }
    else if (argc == 1) {	// bp
        printf("\t\tbreakpoint\n");
        printf("\t\t----------\n");
        for (int i = 0; i < bp_idx; i++) {
            printf("\t\t%X\n", bp[i]);
        }
        return 0;
    }
    else {	// bp [address]
        loc = strtol(par, NULL, 16);

        if (loc > total_length || loc < 0) {
            printf("Break point ERROR: out of range!\n");
            return -1;
        }
	// save breakpoint in bp array
        bp[bp_idx] = loc;
        bp_idx++;
        printf("\t\t[ok] create breakpoint %x\n", loc);
    }
    return 0;
}

int run() {
    reg_t r1, r2;
    int ni, xbpe;
    int opcode, mnem;
    int temp, format = 0;
    char* ptr, *form;
    

    while (PC < total_length) {
        /* get mnemonic & n, i */
        opcode = mem[PC];
        ni = opcode % 4;
        mnem = opcode - ni;

        ptr = findmnemonic(mnem);
        form = findFormat(ptr);     // find mnemonic's format

	/* for debugging */
	//printf("running at [%X], instruction [%s], format [%s]\n", PC, ptr, form);
        
	/* set x, b, p, e*/
        xbpe = mem[PC + 1];
        xbpe = xbpe >> 4;

        /* set format*/
        if (strcmp(form, "3/4") == 0) {
            if (xbpe == 0x1) {
                format = 4;
            }
            else format = 3;
        }
        else if (strcmp(form, "2") == 0) {
            format = 2;
        }
        else if (strcmp(form, "1") == 0) {
            format = 1;
        }

        PC += format;

        /* operate */
        if (format == 1);
        else if (format == 2) {
            temp = mem[(PC - 2) + 1];
            r2 = temp & 0x0000000F;
            r1 = temp & 0x000000F0;
            r1 = r1 >> 4;
            form2_operation(ptr, r1, r2);
        }
        else if (format == 3) {
            temp = getValfromMemory(PC - 3, format);
            if (xbpe == 2) {
		// pc relative
                temp += PC;
            }
            else if (xbpe == 4) {
		// base relative
                temp += B;
            }
            else if (xbpe == 10) {
		// pc + indexed
                temp += PC;
            }
            else if (xbpe == 12) {
		// base + indexed
                temp += B;
            }
            operation(ptr, temp, ni);
        }
        else if (format == 4) {
            temp = getValfromMemory(PC - 4, format);
            operation(ptr, temp, ni);
        }
        if (PC == total_length) {
            printf(" A : %06X  X : %06X\n", A, X);
            printf(" L : %06X PC : %06X\n", L, PC);
            printf(" B : %06X  S : %06X\n", B, S);
            printf(" T : %06X\n", T);
            printf("\t\tEnd Program\n");
            return 0;
        }
        for (int i = 0; i < bp_idx; i++) {
            if (PC == bp[i]) {
                printf(" A : %06X  X : %06X\n", A, X);
                printf(" L : %06X PC : %06X\n", L, PC);
                printf(" B : %06X  S : %06X\n", B, S);
                printf(" T : %06X\n", T);
                printf("\t\tStop at checkpoint[%X]\n", PC);
                return 0;
            }
        }
    }
    return 0;
}

char* findmnemonic(int op) {
    struct HashNode* ptr = table[0];
    // search target in hashtable
    for (int i = 0; i < 20; i++) {
        for (ptr = table[i]; ptr != NULL; ptr = ptr->next) {
            if (ptr->num == op) {
                return ptr->inst;
            }
        }
    }

    // if not found -> error
    return NULL;
}

reg_t findReg(reg_t r) {
    // return register
    switch (r) {
    case 0:
        return A;
    case 1:
        return X;
    case 2:
        return L;
    case 8:
        return PC;
    case 3:
        return B;
    case 4:
        return S;
    case 5:
        return T;
    default:
        break;
    }

    return -1;
}

void form2_operation(char *inst, reg_t r1, reg_t r2) {
    // do operation of format 2 instruction
    if (strcmp(inst, "COMPR") == 0) {
        r1 = findReg(r1);
        r2 = findReg(r2);
        if (r1 == r2) CC = '=';
        else if (r1 < r2) CC = '<';
        else CC = '>';
    }
    else if (strcmp(inst, "CLEAR") == 0) {
        switch (r1) {
        case 0:
            A = 0;
            break;
        case 1:
            X = 0;
            break;
        case 2:
            L = 0;
            break;
        case 8:
            PC = 0;
            break;
        case 3:
            B = 0;
            break;
        case 4:
            S = 0;
            break;
        case 5:
            T = 0;
            break;
        default:
            break;
        }
    }
    else if (strcmp(inst, "TIXR") == 0) {
        r1 = findReg(r1);
        X += 1;
        if (X == r1) CC = '=';
        else if (X < r1) CC = '<';
        else CC = '>';
    }
}

int getValfromMemory(int loc, int format) {
    // get value from memory
    int val = 0;
    if (format == 4) {
        val += (mem[loc + 1] % 16);
        val = val << 16;
        val += (mem[loc + 2] << 8);
        val += mem[loc + 3];
    }
    else if (format == 3) {
        val += mem[loc + 1] % 16;
        val = val << 8;
        if ((val & 0x00000800) == 0x00000800)
            val += 0xFFFFF000;
        val += mem[loc + 2];
    }

    return val;
}

void operation(char *inst, int addr, int ni) {
    // do operation of format 3/4 instruction
    int val = 0;
    
    if (strcmp(inst, "STL") == 0) {
        mem[addr] = (L & 0x00FF0000) >> 16;
        mem[addr + 1] = (L & 0x0000FF00) >> 8;
        mem[addr + 2] = (L & 0x000000FF);
    }
    else if (strcmp(inst, "STA") == 0) {
        mem[addr] = (A & 0x00FF0000) >> 16;
        mem[addr + 1] = (A & 0x0000FF00) >> 8;
        mem[addr + 2] = (A & 0x000000FF);
    }
    else if (strcmp(inst, "STCH") == 0) {
        mem[addr + X] = A & 0x000000FF;
    }
    else if (strcmp(inst, "STX") == 0) {
        mem[addr] = (X & 0x00FF0000) >> 16;
        mem[addr + 1] = (X & 0x0000FF00) >> 8;
        mem[addr + 2] = (X & 0x000000FF);
    }
    else if (strcmp(inst, "LDB") == 0) {
        if (ni != 1) {
            val += mem[addr] << 16;
            val += mem[addr + 1] << 8;
            val += mem[addr + 2];
            B = val;
        }
        else {
            B = addr;
        }
    }
    else if (strcmp(inst, "LDA") == 0) {
        if (ni != 1) {
            val += mem[addr] << 16;
            val += mem[addr + 1] << 8;
            val += mem[addr + 2];
            A = val;
        }
        else {
            A = addr;
        }
    }
    else if (strcmp(inst, "LDT") == 0) {
        if (ni != 1) {
            val += mem[addr] << 16;
            val += mem[addr + 1] << 8;
            val += mem[addr + 2];
            T = val;
        }
        else {
            T = addr;
        }
    }
    else if (strcmp(inst, "LDCH") == 0) {
        A = A & 0xFFFFFF00;
        A += mem[addr + X] & 0x000000FF;
    }
    else if (strcmp(inst, "JSUB") == 0) {
        L = PC;
        PC = addr;
    }
    else if (strcmp(inst, "JEQ") == 0) {
        if (CC == '=') {
            PC = addr;
        }
    }
    else if (strcmp(inst, "J") == 0) {
        if (ni == 2) {
            val += mem[addr] << 16;
            val += mem[addr + 1] << 8;
            val += mem[addr + 2];
            PC = val;
        }
        else PC = addr;
    }
    else if (strcmp(inst, "JLT") == 0) {
        if (CC == '<') {
            PC = addr;
        }
    }
    else if (strcmp(inst, "COMP") == 0) {
        if (A == addr) CC = '=';
        else if (A < addr) CC = '<';
        else CC = '>';
    }
    
    else if (strcmp(inst, "TD") == 0) {
        CC = '<';
    }
    else if (strcmp(inst, "RD") == 0) {
        CC = '=';
    }
    else if (strcmp(inst, "WD") == 0) {
        return;
    }
    else if (strcmp(inst, "RSUB") == 0) {
        PC = L;
    }
}
