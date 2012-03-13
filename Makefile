# Multi-platform makefile for SPARToR

# Get system name
UNAME := $(shell sh -c 'uname -s 2>/dev/null || echo not')

# Same for all platforms, probably
CC = gcc
OBJS = engine/main.o \
       engine/font.o \
       engine/console.o \
       engine/command.o \
       engine/net.o \
       engine/host.o \
       engine/client.o \
       engine/input.o \
       engine/video.o \
       engine/audio.o \
       engine/sjdl.o \
       engine/patt.o \
       engine/saveload.o \
       engine/sprite.o \
       engine/sprite_helpers.o \
       engine/projection.o \
       engine/sjglob.o \
       engine/helpers.o \
       engine/mt19937ar/mt19937ar.o
FLAGS = --std=c99 -g -Wall -Wextra -Wno-unused-parameter -Wno-overlength-strings -pedantic -DGLEW_STATIC
INC = -Iengine -Igame

# Only useful on certain platforms
OBJSRES =
WINDRES =
POSTCC =

# Game / Mod stuff
include game/Makefile-include


ifeq ($(UNAME),Linux)
	EXE_NAME = spartor_linux
	FLAGS += `sdl-config --cflags`
	LIBS = -lm -lSDL -lSDL_net -lSDL_image -lGL -lGLU -lGLEW
endif
ifeq ($(UNAME),Darwin)
	EXE_NAME = platforms/mac/spartor_mac.app/Contents/MacOS/spartor_mac
	FLAGS += `sdl-config --cflags`
	LIBS = -lm `sdl-config --libs` -lSDL_net -lSDL_image -framework OpenGL -lGLEW
	POSTCC = cp -R -f platforms/mac/spartor_mac.app .
endif
ifneq (,$(findstring MINGW,$(UNAME)))
	EXE_NAME = spartor_win32.exe
	OBJSRES = game/icon.o
	WINDRES = windres
	FLAGS += -mwindows
	LIBS = -L/local/lib -Lplatforms/win/glew-1.5.8/lib \
	       -lmingw32 -lSDLmain -lSDL -lSDL_net -lSDL_image -lglew32s -lopengl32 -lglu32 -lm
	INC += -I/usr/local/include/SDL -Iplatforms/win/glew-1.5.8/include
	POSTCC = cp platforms/win/*.dll .
endif


$(EXE_NAME): $(OBJS) $(OBJSRES)
	$(CC) -o $@ $(OBJS) $(OBJSRES) $(FLAGS) $(INC) $(LIBS) $(XLIBS)
	$(POSTCC)
	-rm $(OBJS) $(OBJSRES)

$(OBJS):
	$(CC) -o $@ -c $(FLAGS) $(INC) $*.c

.rc.o:
	$(WINDRES) $^ -o $@
%.o : %.rc
	$(WINDRES) $^ -o $@

clean:
	-rm $(OBJS) $(OBJSRES)

