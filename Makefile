# Windows with MinGW
CC=x86_64-w64-mingw32-gcc
CFLG=-O3 -Wall -DUSEGLEW -I"C:/msys64/mingw64/include"
LIBS=-L"C:/msys64/mingw64/lib" -lfreeglut -lglew32 -lglu32 -lopengl32 -lwinmm -lgdi32
LDFLAGS=-static-libgcc -static-libstdc++
CLEAN=del /F /Q *.exe *.o *.a src\*.o RocketWrangler3D.exe 2>NUL

# Where to place all object files
OBJDIR=tmp/obj

#  Source files and objects
SRCS = src/run.c \
	$(wildcard src/draw/*.c) \
	$(wildcard src/game/*.c) \
	$(wildcard assets/Shapes/*.c) \
	$(wildcard assets/Models/*.c)
# Put objects under $(OBJDIR), mirroring source subfolders
OBJS = $(patsubst %.c,$(OBJDIR)/%.o,$(SRCS))

#  Compile and link
RocketWrangler3D: $(OBJS)
	$(CC) $(CFLG) -o $@ $(OBJS) $(LIBS) $(LDFLAGS)

#  Generic rule for compiling objects into $(OBJDIR)
#  Create the output directory on Windows cmd-compatible way
$(OBJDIR)/%.o: %.c
	@if not exist "$(dir $@)" mkdir "$(dir $@)"
	$(CC) $(CFLG) -c $< -o $@

#  Clean
clean:
	-del /F /Q *.exe *.o *.a 2>NUL
	-del /F /Q src\*.o src\draw\*.o src\game\*.o assets\Shapes\*.o assets\Models\*.o 2>NUL
	-@if exist "$(OBJDIR)" rmdir /S /Q "$(OBJDIR)" 2>NUL
	-del /F /Q RocketWrangler3D.exe 2>NUL

#  Distribution
dist: RocketWrangler3D
	-if not exist dist mkdir dist
	copy /Y RocketWrangler3D.exe dist\RocketWrangler3D.exe
	REM FreeGLUT DLL (name differs by install) — copy if present
	if exist "C:\msys64\mingw64\bin\freeglut.dll" copy /Y "C:\msys64\mingw64\bin\freeglut.dll" dist\freeglut.dll
	if exist "C:\msys64\mingw64\bin\libfreeglut.dll" copy /Y "C:\msys64\mingw64\bin\libfreeglut.dll" dist\libfreeglut.dll
	REM GLEW DLL (name differs by install) — copy if present
	if exist "C:\msys64\mingw64\bin\glew32.dll" copy /Y "C:\msys64\mingw64\bin\glew32.dll" dist\glew32.dll
	if exist "C:\msys64\mingw64\bin\libglew32.dll" copy /Y "C:\msys64\mingw64\bin\libglew32.dll" dist\libglew32.dll
	REM Pthreads runtime used by MinGW for timing/APIs — copy if present
	if exist "C:\msys64\mingw64\bin\libwinpthread-1.dll" copy /Y "C:\msys64\mingw64\bin\libwinpthread-1.dll" dist\libwinpthread-1.dll
	REM In case static flags are removed in future, include runtimes if needed
	if exist "C:\msys64\mingw64\bin\libgcc_s_seh-1.dll" copy /Y "C:\msys64\mingw64\bin\libgcc_s_seh-1.dll" dist\libgcc_s_seh-1.dll
	if exist "C:\msys64\mingw64\bin\libstdc++-6.dll" copy /Y "C:\msys64\mingw64\bin\libstdc++-6.dll" dist\libstdc++-6.dll
