# Windows with MinGW
CC=x86_64-w64-mingw32-gcc
CFLG=-O3 -Wall -DUSEGLEW -I"C:/msys64/mingw64/include"
LIBS=-L"C:/msys64/mingw64/lib" -lfreeglut -lglew32 -lglu32 -lopengl32
LDFLAGS=-static-libgcc -static-libstdc++
CLEAN=del /F /Q *.exe *.o *.a src\*.o RocketWrangler3D.exe 2>NUL

#  Source files and objects
SRCS = src/run.c \
       $(wildcard src/draw/*.c) \
       $(wildcard src/game/*.c) \
       $(wildcard assets/Shapes/*.c)
OBJS = $(SRCS:.c=.o)

#  Compile and link
RocketWrangler3D: $(OBJS)
	$(CC) $(CFLG) -o $@ $(OBJS) $(LIBS) $(LDFLAGS)

#  Generic rule for compiling objects
%.o: %.c
	$(CC) $(CFLG) -c $< -o $@

#  Clean
clean:
	$(CLEAN)
	rm -f $(OBJS) RocketWrangler3D src/*.o
