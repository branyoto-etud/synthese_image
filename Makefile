CC = gcc
CFLAGS = -ansi -Wall
LDFLAGS = -lm -lrt $(libG3X)
EXEC = Main
OBJ = Main.o Camera.o Object.o Rayon.o CameraConfig.o Utils.o
inc = -I./include $(incG3X)

$(EXEC): $(OBJ)
	$(CC) -o $@ $^ $(LDFLAGS)

Object.o: ./src/Object.c ./include/Object.h
Utils.o: ./src/Utils.c ./include/Utils.h
CameraConfig.o: ./src/CameraConfig.c ./include/CameraConfig.h
Rayon.o: ./src/Rayon.c ./include/Rayon.h ./include/Object.h ./include/Utils.h
Main.o: ./src/Main.c ./include/Camera.h ./include/Object.h ./include/CameraConfig.h
Camera.o: ./src/Camera.c ./include/Camera.h ./include/Object.h ./include/Rayon.h ./include/CameraConfig.h

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
	./bin/Main

uninstall: mrproper
	rm -f ./bin/*
	rm -df ./bin

all: uninstall install
