#include "20172141.h"

int edit(){
  if(argc > 3) return -1;    // 필요한 argument 갯수보다 입력된 것이 많은 경우 error
	// argument 16진수로 변환
  v[0] = strtol(argv[1], NULL, 16);
	v[1] = strtol(argv[2], NULL, 16);
	
	int idx = v[0], val = v[1];
	if (idx > 0xFFFFF) return -1;
	mem[idx] = val;

	return 0;
}

int fill(){
	int i;
  if(argc > 4) return -1;    // 필요한 argument 갯수보다 입력된 것이 많은 경우 error
	// argument 16진수로 변환
  v[0] = strtol(argv[1], NULL, 16);
	v[1] = strtol(argv[2], NULL, 16);
	v[2] = strtol(argv[3], NULL, 16);
	
	int s = v[0], e = v[1], val = v[2];
	if (s > 0xFFFFF || e > 0xFFFFF) return -1;    // 잘못된 index -> error
	for(i = s; i < e; i++){
		mem[i] = val;
	}

	return 0;
}

int dump(){
	int err = -1;
	int i, j;
	int s, e;	// s: start, e: end
	s = e = 0;
	
	if(argc == 1){		// dump
		s = addr; e = addr +160;
		if(s >= MEMSIZE) s = 0;
		if(e >= MEMSIZE) e = MEMSIZE;
		row = s / 16;
    
    //print
		for(i = row; i < e/16; i++){
			printf("%05X ", i*16);
			for(j = 0; j < 16; j++){
				int idx = i * 16 + j;
				printf("%02X ", mem[idx]);
			}
			printf("; ");
			for(j = 0; j < 16; j++){
				int idx = i * 16 + j;
				if(mem[idx] >= 32 && mem[idx] <= 126){
					printf("%c", mem[idx]);
				}
				else printf(".");
			}
			printf("\n");
		}
		addr = e;    // address update
		if(addr >= MEMSIZE) addr = 0;
	}
	else if(argc == 2){		// dump start
    // argument 16진수 변환
  	v[0] = strtol(argv[1], NULL, 16);
		s = v[0]; e = v[0] + 160; 
		if(s >= MEMSIZE) return err;
		if(e >= MEMSIZE) e = MEMSIZE;
   
		row = s / 16;
		if(s % 16 != 0) erow = e / 16 + 1;
		else erow = e / 16;
    if (erow > MAX_ROW_SIZE) erow = MAX_ROW_SIZE;
	
    //print
  	for(i = row; i < erow; i++){
			printf("%05X ", i*16);
			for(j = 0; j < 16; j++){
				int idx = i*16 + j;
				if(idx < s || idx > e) printf("   ");
				else printf("%02X ", mem[idx]);
			}
			printf("; ");
			for(j = 0; j < 16; j++){
				int idx = i*16 + j;
				if(idx < s || idx > e) printf(" ");
				else{
					if(mem[idx] >= 32 && mem[idx] <= 126){
						printf("%c", mem[idx]);
					}
					else printf(".");
				}
			}
			printf("\n");
		}
	}
	else if(argc == 3){		// dump start, end
		// argument 16진수 변환
    v[0] = strtol(argv[1], NULL, 16);
		v[1] = strtol(argv[2], NULL, 16);
		s = v[0]; e = v[1];
		if(s > e || e > 0xFFFFF) return err;
		
    row = s/16;
		erow = e / 16 + 1;
    if (erow > MAX_ROW_SIZE) erow = MAX_ROW_SIZE;

    //print
    for(i = row; i < erow; i++){
			printf("%05X ", i*16);
			for(j=0; j<16; j++){
				int idx = i*16 + j;
				if(idx < s || idx > e) printf("   ");
				else printf("%02X ", mem[idx]);
			}
			printf("; ");
			for(j=0; j<16; j++){
				int idx = i*16 + j;
				if(idx < s || idx > e) printf(" ");
				else{
					if(mem[idx] >= 32 && mem[idx] <= 126){
						printf("%c", mem[idx]);
					}
					else printf(".");
				}
			}
			printf("\n");
		}
	}

	return 0;
}
