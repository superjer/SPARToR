# Multi-platform makefile for SPARToR

# Get system name
UNAME := $(shell sh -c 'uname -s 2>/dev/null || echo not')

# Same for all platforms, probably
CC = gcc
MKDIRP = mkdir -p
OBJDIR_MC = obj-mc
OBJDIR_DK = obj-dk
SRCS = $(wildcard engine/*.c)
SRCS += $(wildcard engine/mt19937ar/*.c)
SRCS_MC = $(SRCS) $(wildcard mcdiddy/*.c)
SRCS_DK = $(SRCS) $(wildcard deadking/*.c)
OBJS_MC = $(patsubst %.c,$(OBJDIR_MC)/%.o,$(SRCS_MC))
OBJS_DK = $(patsubst %.c,$(OBJDIR_DK)/%.o,$(SRCS_DK))
DEPS_MC = $(OBJS_MC:.o=.d)
DEPS_DK = $(OBJS_DK:.o=.d)

GITCOMMIT := $(shell sh -c "git branch -v | grep '^\*' | sed 's/\s\+/ /g' | cut -d' ' -f2,3")

FLAGS = -g -Wall -Wextra -Werror -Wno-unused-parameter -Wno-overlength-strings -pedantic -DGLEW_STATIC
FLAGS += -DGITCOMMIT='"$(GITCOMMIT)"'
FLAGS += -std=c99

INC = -Iengine

# Only useful on certain platforms
OBJSRES =
WINDRES =
POSTCC =
EXE_PATH = ./
EXE_SUF = .bin

ifeq ($(UNAME),Linux)
	FLAGS += -DDOGLEW `sdl2-config --cflags`
	LIBS = -lm -lSDL2 -lSDL2_net -lSDL2_image -lGL -lGLU -lGLEW
endif
ifeq ($(UNAME),Darwin)
	EXE_PATH = platforms/mac/spartor.app/Contents/MacOS/
	# Because GLU funcs are deprecated on Mac (?):
	FLAGS += -Wno-deprecated-declarations
	FLAGS += -I/Library/Frameworks/SDL2.framework/Headers
	FLAGS += -I/Library/Frameworks/SDL2_net.framework/Headers
	FLAGS += -I/Library/Frameworks/SDL2_image.framework/Headers
	LIBS = -lm -framework SDL2 -framework SDL2_net -framework SDL2_image -framework OpenGL
	POSTCC = cp -R -f platforms/mac/spartor.app .
endif
ifneq (,$(findstring MINGW,$(UNAME)))
	EXE_SUF = .exe
	OBJSRES_MC = mcdiddy/icon.o
	OBJSRES_DK = deadking/icon.o
	WINDRES = windres
	FLAGS += -DDOGLEW -mwindows
	LIBS = -Lplatforms/win32/lib
	LIBS += -lmingw32 -lSDL2main -lSDL2 -lSDL2_net -lSDL2_image -lglew32 -lopengl32 -lglu32 -lm
	INC += -Iplatforms/win32/include -Iplatforms/win32/include/SDL2
	POSTCC = cp platforms/win32/bin/*.dll .
endif

INC_MC = $(INC) -Imcdiddy
INC_DK = $(INC) -Ideadking
EXE_MC = $(EXE_PATH)mcdiddy$(EXE_SUF)
EXE_DK = $(EXE_PATH)deadking$(EXE_SUF)
FLAGS_MC = $(FLAGS) -D'GAME="mcdiddy"'
FLAGS_DK = $(FLAGS) -D'GAME="deadking"'

all: $(EXE_MC) $(EXE_DK)

$(EXE_MC): $(OBJS_MC) $(OBJSRES_MC)
	$(CC) -o $@ $(OBJS_MC) $(OBJSRES_MC) $(FLAGS_MC) $(INC_MC) $(LIBS) $(XLIBS)
	$(POSTCC)

$(EXE_DK): $(OBJS_DK) $(OBJSRES_DK)
	$(CC) -o $@ $(OBJS_DK) $(OBJSRES_DK) $(FLAGS_DK) $(INC_DK) $(LIBS) $(XLIBS)
	$(POSTCC)

-include $(DEPS_MC)
-include $(DEPS_DK)

$(OBJDIR_MC)/%.o: %.c
	@$(MKDIRP) $(dir $@)
	$(CC) $(FLAGS_MC) $(INC_MC) -MM -MT $@ -MF $(patsubst %.o,%.d,$@) $<
	$(CC) -o $@ -c $(FLAGS_MC) $(INC_MC) $<

$(OBJDIR_DK)/%.o: %.c
	@$(MKDIRP) $(dir $@)
	$(CC) $(FLAGS_DK) $(INC_DK) -MM -MT $@ -MF $(patsubst %.o,%.d,$@) $<
	$(CC) -o $@ -c $(FLAGS_DK) $(INC_DK) $<

.rc.o:
	$(WINDRES) $^ -o $@
%.o : %.rc
	$(WINDRES) $^ -o $@

clean:
	-$(RM) $(OBJS_MC) $(OBJS_DK) $(OBJSRES_MC) $(OBJSRES_DK)

distclean: clean
	-$(RM) -r $(EXE_MC) $(EXE_DK) $(OBJDIR_MC) $(OBJDIR_DK)
