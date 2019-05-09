CC = gcc
CFLAGS = -Wall -Wextra -std=gnu99 -I.
OBJ = crack.o
EXE = crack
##Make .o files from .c files
%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

##EXE from object file
$(EXE): $(OBJ)
	gcc -o $@ $^ $(CFLAGS)

#Delete objects
clean:
	/bin/rm $(OBJ)

##preforms clean
clobber: clean
	/bin/rm $(EXE)
