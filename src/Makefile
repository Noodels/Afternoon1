CC=gcc
EXECNAME = afternoon1
OBJ = microcode.o stack.o blu.o memory.o cache.o central.o bus.o
MAIN = main.o
TEST = test.o

all: afternoon1

afternoon1: $(OBJ) $(MAIN)
	$(CC) -o $(EXECNAME) $(OBJ) $(MAIN)

# Testing program
test: $(OBJ) $(TEST)
	$(CC) -o test $(OBJ) $(TEST)

clean:
	rm -f *.o
	rm -f $(EXECNAME)
	rm -f test
