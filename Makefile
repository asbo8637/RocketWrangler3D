#  MinGW
ifeq "$(OS)" "Windows_NT"
CFLG=-O3 -Wall -DUSEGLEW
LIBS=-lfreeglut -lglew32 -lglu32 -lopengl32
CLEAN=rm *.exe *.o *.a
else
#  OSX
ifeq "$(shell uname)" "Darwin"
CFLG=-O3 -Wall -Wno-deprecated-declarations -DRES=2
LIBS=-framework GLUT -framework OpenGL
#  Linux/Unix/Solaris
else
CFLG=-O3 -Wall
LIBS=-lglut -lGLU -lGL -lm
endif
#  OSX/Linux/Unix/Solaris
CLEAN=rm -f gears *.o *.a
endif

#  Source files and objects
SRCS = src/run.c src/drawScene.c src/camera.c src/controls.c src/engine.c src/renderer.c
OBJS = $(SRCS:.c=.o)

#  Compile and link
RocketWrangler3D: $(OBJS)
	gcc $(CFLG) -o $@ $(OBJS) $(LIBS)

#  Dependencies
src/run.o: src/run.c src/camera.h src/controls.h src/renderer.h
src/camera.o: src/camera.c src/camera.h
src/controls.o: src/controls.c src/controls.h src/camera.h
src/drawScene.o: src/drawScene.c src/camera.h
src/engine.o: src/engine.c
src/renderer.o: src/renderer.c src/renderer.h src/camera.h

#  Generic rule for compiling objects
%.o: %.c
	gcc $(CFLG) -c $< -o $@

#  Clean
clean:
	$(CLEAN)
	rm -f $(OBJS) RocketWrangler3D src/*.o
