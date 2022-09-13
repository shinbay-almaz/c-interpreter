# Auto-Generated Makefile

CC = gcc
CFLAGS = -Wall -std=c11
OBJ = scanner.o memory.o vm.o debug.o main.o chunk.o compiler.o value.o 
DEPS = debug.h compiler.h chunk.h value.h common.h scanner.h memory.h vm.h 

%.o : %.c $(DEPS)
	$(CC) $(CFLAGS) -c -o $@ $<

run: $(OBJ)
	$(CC) $(CFLAGS) -o run $^
