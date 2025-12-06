# Makefile scaffold produced with AI assistance and lightly customized for this project.
# Compiler / flags
CC ?= gcc
USE_GLEW ?= 0
CFLG ?= -O3 -Wall
ifeq ($(USE_GLEW),1)
	CFLG += -DUSEGLEW
endif
LDFLAGS ?=

# Platform-specific libs and executable suffix
ifeq ($(OS),Windows_NT)
	EXEC=RocketWrangler3D.exe
	LIBS=-lfreeglut -lopengl32 -lglu32 -lgdi32 -lm
	ifeq ($(USE_GLEW),1)
		LIBS += -lglew32
	endif
	RM=rm -f
	MKDIR=mkdir -p
else
	EXEC=RocketWrangler3D
	LIBS=-lglut -lGLU -lGL -lm -lpthread
	ifeq ($(USE_GLEW),1)
		LIBS += -lGLEW
	endif
	RM=rm -f
	MKDIR=mkdir -p
endif

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
$(EXEC): $(OBJS)
	$(CC) $(CFLG) -o $@ $(OBJS) $(LIBS) $(LDFLAGS)
	@$(MKDIR) dist
	cp -f $(EXEC) dist/$(EXEC)

#  Generic rule for compiling objects into $(OBJDIR)
#  Create the output directory with Unix mkdir -p
$(OBJDIR)/%.o: %.c
	@$(MKDIR) $(dir $@)
	$(CC) $(CFLG) -c $< -o $@

#  Clean
clean:
	-$(RM) *.exe *.o *.a
	-$(RM) src/*.o src/draw/*.o src/game/*.o assets/Shapes/*.o assets/Models/*.o assets/Models/Robot_positions/*.o
	-rm -rf "$(OBJDIR)"
	-$(RM) $(EXEC) RocketWrangler3D

#  Distribution (Linux - just copy the executable)
dist: $(EXEC)
	mkdir -p dist
	cp -f $(EXEC) dist/$(EXEC)
