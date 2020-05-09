# Name: Sally Chung
# Project: Lab 6: Wack-a-mole.

EXECUTABLES=spiritd
SRC=spiritd.c moles.c
OBJ=spiritd.o moles.o

CC=gcc
CFLAGS=-g

all: $(EXECUTABLES)

	$(EXECUTABLES): $(SRC)
	$(CC) $(CFLAGS) -c $(SRC)
	$(CC) $(CFLAGS) -o $@ $(OBJ) -lpthread -lrt

spiritd: spiritd.c
	gcc -g -c spiritd.c
	gcc -g -o $@ spiritd.o -lpthread -lrt

moles: moles.c
	gcc -g -c moles.c
	gcc -g -o $@ moles.o -lpthread -lrt

clean:
	rm -f $(EXECUTABLES) $(OBJ) 