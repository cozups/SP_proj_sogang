20172141.out : 20172141.o shell.o memory.o opcode.o
	gcc -Wall -g -o 20172141.out 20172141.o shell.o memory.o opcode.o
20172141.o:
	gcc -Wall -g -c 20172141.c
shell.o:
	gcc -Wall -g -c shell.c
memory.o:
	gcc -Wall -g -c memory.c
opcode.o:
	gcc -Wall -g -c opcode.c
clean:
	rm -f *.o 20172141.out
