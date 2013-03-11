CC=g++

CFLAGS=-c

all: clear_screen

clear_screen: main.o ogl_es.o clear_screen.o
	g++ main.o ogl_es.o clear_screen.o -lX11 -lGL -lGLEW -o clear_screen
    
main.o: ../main.cpp
	$(CC) $(CFLAGS) ../main.cpp
	
ogl_es.o: ../ogl_es.cpp
	$(CC) $(CFLAGS) ../ogl_es.cpp
	
clear_screen.o: clear_screen.cpp
	$(CC) $(CFLAGS) clear_screen.cpp
    
clean:
	rm -rf *.o clear_screen