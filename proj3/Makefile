20172141.out : 20172141.o shell.o memory.o opcode.o assemble.o loader.o
	gcc -Wall -g -o 20172141.out 20172141.o shell.o memory.o opcode.o assemble.o loader.o
20172141.o : 20172141.c
	gcc -Wall -g -c 20172141.c
shell.o : shell.c
	gcc -Wall -g -c shell.c
memory.o : memory.c
	gcc -Wall -g -c memory.c
opcode.o : opcode.c
	gcc -Wall -g -c opcode.c
assemble.o : assemble.c
	gcc -Wall -g -c assemble.c
loader.o : loader.c
	gcc -Wall -g -c loader.c
clean:
	rm -f *.o *.lst 20172141.out

