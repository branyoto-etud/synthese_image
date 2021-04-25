CC = gcc
CFLAGS = -ansi -Wall
LDFLAGS = -lm -lrt $(libG3X)
EXEC = Main
OBJ = Main.o Camera.o Objet.o Rayon.o
inc = -I./include $(incG3X)

$(EXEC): $(OBJ)
	$(CC) -o $@ $^ $(LDFLAGS)

Main.o: ./src/Main.c ./include/Camera.h
Camera.o: ./src/Camera.c ./include/Camera.h
Objet.o: ./src/Objet.c ./include/Objet.h
Rayon.o: ./src/Rayon.c ./include/Rayon.h ./include/Objet.h

%.o: ./src/%.c
	$(CC) -c $< $(CFLAGS) $(inc)

clean:
	rm -f *.o

mrproper: clean
	rm -f $(EXEC)

install: $(EXEC)
	mkdir ./bin
	mv $(EXEC) ./bin
	make clean

uninstall: mrproper
	rm -f ./bin/*
	rm -df ./bin

all: uninstall install
