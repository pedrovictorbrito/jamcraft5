SRCDIR=./src/main.c
IDIR =/usr/include
LDIR=/usr/lib
LIBS=-lSDL2 -lSDL2_ttf
ODIR=./bin/
OBJ=main
CC=gcc
CFLAGS=-I$(IDIR) -B$(LDIR) $(LIBS) -o$(ODIR)$(OBJ) $(SRCDIR)

run:
	$(CC) -D PATH_SEPARATOR="'/'" $(CFLAGS)
	bin/main

linux:
	$(CC) -D PATH_SEPARATOR="'/'" $(CFLAGS)

clean:
	rm -f $(ODIR)$(OBJ)
