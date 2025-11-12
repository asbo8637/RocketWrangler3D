# Linux / WSL
CC=gcc
CFLG=-O3 -Wall -DUSEGLEW
LIBS=-lglut -lGLEW -lGLU -lGL -lm -lpthread
LDFLAGS=
CLEAN=rm -f *.exe *.o *.a src/*.o RocketWrangler3D

# Where to place all object files
OBJDIR=tmp/obj

#  Source files and objects
SRCS = src/run.c \
	$(wildcard src/draw/*.c) \
	$(wildcard src/game/*.c) \
	$(wildcard assets/Shapes/*.c) \
	$(wildcard assets/Models/*.c) \
	$(wildcard assets/Models/Robot_positions/*.c)
# Put objects under $(OBJDIR), mirroring source subfolders
OBJS = $(patsubst %.c,$(OBJDIR)/%.o,$(SRCS))

#  Compile and link
RocketWrangler3D: $(OBJS)
	$(CC) $(CFLG) -o $@ $(OBJS) $(LIBS) $(LDFLAGS)

#  Generic rule for compiling objects into $(OBJDIR)
#  Create the output directory with Unix mkdir -p
$(OBJDIR)/%.o: %.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLG) -c $< -o $@

#  Clean
clean:
	-rm -f *.exe *.o *.a
	-rm -f src/*.o src/draw/*.o src/game/*.o assets/Shapes/*.o assets/Models/*.o assets/Models/Robot_positions/*.o
	-rm -rf "$(OBJDIR)"
	-rm -f RocketWrangler3D

#  Distribution (Linux - just copy the executable)
dist: RocketWrangler3D
	mkdir -p dist
	cp -f RocketWrangler3D dist/RocketWrangler3D
