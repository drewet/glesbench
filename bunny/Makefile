CC=g++
CFLAGS=-c
OBJS=main.o xgl.o utils.o gccxnamath.o fraps.o shader.o bunny.o
LIBS=-lX11 -lGL -lGLEW
LDFLAGS=$(LIBS)

all: bunny

bunny: $(OBJS) vsh.h fsh.h ../ogl_es.h
	$(CC) $(OBJS) $(LDFLAGS) -o bunny
    
main.o: ../X11/main.cpp
	$(CC) $(CFLAGS) ../X11/main.cpp
	
xgl.o: ../X11/xgl.cpp
	$(CC) $(CFLAGS) ../X11/xgl.cpp
	
utils.o: ../utils.cpp
	$(CC) $(CFLAGS) ../utils.cpp
	
gccxnamath.o: ../xna/gccxnamath.cpp
	$(CC) $(CFLAGS) ../xna/gccxnamath.cpp
	
fraps.o: fraps.cpp fraps.h
	$(CC) $(CFLAGS) fraps.cpp
	
shader.o: shader.cpp shader.h
	$(CC) $(CFLAGS) shader.cpp
	
bunny.o: bunny.cpp vsh.h fsh.h
	$(CC) $(CFLAGS) bunny.cpp
    
clean:
	rm -rf *.o bunny