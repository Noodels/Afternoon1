CC=gcc
EXECNAME = afternoon1
OBJ = main.o microcode.o stack.o blu.o memory.o cache.o central.o

all: afternoon1

afternoon1: $(OBJ)
	$(CC) -o $(EXECNAME) $(OBJ)

clean:
	rm -f *.o
	rm -f $(EXECNAME)
