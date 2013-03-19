CC=g++
CFLAGS=-c
OBJS=main.o xgl.o clear_screen.o
LIBS=-lX11 -lGL -lGLEW
LDFLAGS=$(LIBS)

all: clear_screen

clear_screen: $(OBJS)
	$(CC) $(OBJS) $(LDFLAGS) -o clear_screen
    
main.o: ../X11/main.cpp
	$(CC) $(CFLAGS) ../X11/main.cpp
	
xgl.o: ../X11/xgl.cpp
	$(CC) $(CFLAGS) ../X11/xgl.cpp

clear_screen.o: clear_screen.cpp ../ogl_es.h
	$(CC) $(CFLAGS) clear_screen.cpp
    
clean:
	rm -rf *.o clear_screen