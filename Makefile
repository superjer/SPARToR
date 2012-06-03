# Multi-platform makefile for SPARToR

# Get system name
UNAME := $(shell sh -c 'uname -s 2>/dev/null || echo not')

# Same for all platforms, probably
CC = gcc
OBJDIR = objects
SRCS = $(wildcard engine/*.c)
SRCS += $(wildcard engine/mt19937ar/*.c)
include game/Makefile-include
OBJS = $(patsubst %.c,$(OBJDIR)/%.o,$(SRCS))

GITCOMMIT := $(shell sh -c "git branch -v | grep '^\*' | sed 's/\s\+/ /g' | cut -d' ' -f2,3")

FLAGS = --std=c99 -g -Wall -Wextra -Wno-unused-parameter -Wno-overlength-strings -pedantic -DGLEW_STATIC
FLAGS += -DGITCOMMIT='"$(GITCOMMIT)"'

INC = -Iengine -Igame

# Only useful on certain platforms
OBJSRES =
WINDRES =
POSTCC =


ifeq ($(UNAME),Linux)
	EXE_NAME = spartor
	FLAGS += `sdl-config --cflags`
	LIBS = -lm -lSDL -lSDL_net -lSDL_image -lGL -lGLU -lGLEW
endif
ifeq ($(UNAME),Darwin)
	EXE_NAME = platforms/mac/spartor.app/Contents/MacOS/spartor
	FLAGS += `sdl-config --cflags`
	LIBS = -lm `sdl-config --libs` -lSDL_net -lSDL_image -framework OpenGL -lGLEW
	POSTCC = cp -R -f platforms/mac/spartor.app .
endif
ifneq (,$(findstring MINGW,$(UNAME)))
	EXE_NAME = spartor.exe
	OBJSRES = game/icon.o
	WINDRES = windres
	FLAGS += -mwindows
	LIBS = -L/usr/local/lib -L/usr/lib \
	       -lmingw32 -lSDLmain -lSDL -lSDL_net -lSDL_image -lglew32 -lopengl32 -lglu32 -lm
	INC += -I/usr/local/include/SDL -I/usr/include
	POSTCC = cp platforms/win/*.dll .
endif

all: clean $(EXE_NAME)

quick: $(EXE_NAME)

$(EXE_NAME): $(OBJS) $(OBJSRES)
	$(CC) -o $@ $(OBJS) $(OBJSRES) $(FLAGS) $(INC) $(LIBS) $(XLIBS)
	$(POSTCC)

$(OBJDIR)/%.o: %.c
	mkdir -p `dirname $@`
	$(CC) -o $@ -c $(FLAGS) $(INC) $*.c

.rc.o:
	$(WINDRES) $^ -o $@
%.o : %.rc
	$(WINDRES) $^ -o $@

clean:
	-$(RM) $(OBJS) $(OBJSRES)

distclean:
	-$(RM) -r $(OBJDIR)
	-$(RM) $(OBJSRES) $(EXE_NAME)
