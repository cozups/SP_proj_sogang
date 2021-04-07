20172141.out : 20172141.o shell.o memory.o opcode.o assemble.o
	gcc -Wall -g -o 20172141.out 20172141.o shell.o memory.o opcode.o assemble.o
20172141.o:
	gcc -Wall -g -c 20172141.c
shell.o:
	gcc -Wall -g -c shell.c
memory.o:
	gcc -Wall -g -c memory.c
opcode.o:
	gcc -Wall -g -c opcode.c
assemble.o:
	gcc -Wall -g -c assemble.c
clean:
	rm -f *.o *.lst *.obj 20172141.out

