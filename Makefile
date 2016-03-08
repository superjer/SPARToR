# Multi-platform makefile for SPARToR

# Get system name
UNAME := $(shell sh -c 'uname -s 2>/dev/null || echo not')

# Same for all platforms, probably
CC = gcc
MKDIRP = mkdir -p
MV = mv
OBJDIR_MC = obj-mc
OBJDIR_DK = obj-dk
SRCS = $(wildcard engine/*.c)
SRCS += $(wildcard engine/mt19937ar/*.c)
SRCS_MC = $(SRCS) $(wildcard mcdiddy/*.c)
SRCS_DK = $(SRCS) $(wildcard deadking/*.c)
SRCS_ENTITY_MC = $(wildcard mcdiddy/entities/*.c)
SRCS_ENTITY_DK = $(wildcard deadking/entities/*.c)
OBJS_MC = $(patsubst %.c,$(OBJDIR_MC)/%.o,$(SRCS_MC))
OBJS_DK = $(patsubst %.c,$(OBJDIR_DK)/%.o,$(SRCS_DK))
OBJS_ENTITY_MC = $(patsubst %.c,$(OBJDIR_MC)/%.o,$(SRCS_ENTITY_MC))
OBJS_ENTITY_DK = $(patsubst %.c,$(OBJDIR_DK)/%.o,$(SRCS_ENTITY_DK))
DEPS_MC = $(OBJS_MC:.o=.d)
DEPS_DK = $(OBJS_DK:.o=.d)
DEPS_ENTITY_MC = $(OBJS_ENTITY_MC:.o=.d)
DEPS_ENTITY_DK = $(OBJS_ENTITY_DK:.o=.d)

GITCOMMIT := $(shell sh -c "git branch -v | grep '^\*' | sed 's/\s\+/ /g' | cut -d' ' -f2,3")

FLAGS = -g -Wall -Wextra -Werror -Wno-unused-parameter -Wno-overlength-strings
FLAGS += -DGLEW_STATIC
FLAGS += -DGITCOMMIT='"$(GITCOMMIT)"'
FLAGS += -std=c99

INC = -Iengine

# Only useful on certain platforms
OBJSRES =
WINDRES =
POSTCC =
EXE_SUF = .bin
DLL_SUF = .so

ifeq ($(UNAME),Linux)
	FLAGS += -DDOGLEW `sdl2-config --cflags`
	LIBS = -lm -ldl -lSDL2 -lSDL2_net -lSDL2_image -lGL -lGLU -lGLEW
endif
ifeq ($(UNAME),Darwin)
	# Because GLU funcs are deprecated on Mac (?):
	FLAGS += -Wno-deprecated-declarations
	FLAGS += -I/Library/Frameworks/SDL2.framework/Headers
	FLAGS += -I/Library/Frameworks/SDL2_net.framework/Headers
	FLAGS += -I/Library/Frameworks/SDL2_image.framework/Headers
	LIBS = -lm -framework SDL2 -framework SDL2_net -framework SDL2_image -framework OpenGL
	POSTCC_MC = cp -R -f platforms/mac/spartor.app mcdiddy.app
	POSTCC_DK = cp -R -f platforms/mac/spartor.app deadking.app
	POSTCC_MC += && sed -e s/spartor/mcdiddy/ -i '' mcdiddy.app/Contents/Info.plist
	POSTCC_DK += && sed -e s/spartor/deadking/ -i '' deadking.app/Contents/Info.plist
	POSTCC_MC += && cp mcdiddy.bin mcdiddy.app/Contents/MacOS/
	POSTCC_DK += && cp deadking.bin deadking.app/Contents/MacOS/
endif
ifneq (,$(findstring MINGW,$(UNAME)))
	EXE_SUF = .exe
	DLL_SUF = .dll
	OBJSRES_MC = mcdiddy/icon.o
	OBJSRES_DK = deadking/icon.o
	WINDRES = windres
	FLAGS += -DDOGLEW -mwindows
	LIBS = -Lplatforms/win32/lib
	LIBS += -lmingw32 -lSDL2main -lSDL2 -lSDL2_net -lSDL2_image -lglew32 -lopengl32 -lglu32 -lm
	INC += -Iplatforms/win32/include -Iplatforms/win32/include/SDL2
	POSTCC = cp platforms/win32/bin/*.dll .
endif
ifneq (,$(findstring MINGW64,$(UNAME)))
        LIBS = -L /mingw64/lib/
        LIBS += -lmingw32 -lSDL2main -lSDL2 -lSDL2_net -lSDL2_image -lglew32 -lopengl32 -lglu32 -lm
        INC += -I/mingw64/include -I/mingw64/include/SDL2
        POSTCC = cp /mingw64/bin/glew32.dll /mingw64/bin/SDL2.dll /mingw64/bin/SDL2_image.dll /mingw64/bin/SDL2_net.dll .
endif


INC_MC = $(INC) -Imcdiddy
INC_DK = $(INC) -Ideadking
EXE_MC = mcdiddy$(EXE_SUF)
EXE_DK = deadking$(EXE_SUF)
DLL_MC = mcdiddy$(DLL_SUF)
DLL_DK = deadking$(DLL_SUF)
FLAGS_MC = $(FLAGS) -D'GAME="mcdiddy"'
FLAGS_DK = $(FLAGS) -D'GAME="deadking"'

all: $(EXE_MC) $(EXE_DK) $(DLL_MC) $(DLL_DK)

mcdiddy: $(EXE_MC)

deadking: $(EXE_DK)

$(EXE_MC): $(OBJS_MC) $(OBJSRES_MC)
	$(CC) -o $@ $(OBJS_MC) $(OBJSRES_MC) $(FLAGS_MC) $(INC_MC) $(LIBS)
	$(POSTCC)
	$(POSTCC_MC)

$(EXE_DK): $(OBJS_DK) $(OBJSRES_DK)
	$(CC) -o $@ $(OBJS_DK) $(OBJSRES_DK) $(FLAGS_DK) $(INC_DK) $(LIBS)
	$(POSTCC)
	$(POSTCC_DK)

$(DLL_MC): $(OBJS_ENTITY_MC)
	$(CC) -shared -o $@.new $(OBJS_ENTITY_MC) -fPIC $(FLAGS_MC) $(INC_MC)
	$(MV) $@.new $@

$(DLL_DK): $(OBJS_ENTITY_DK)
	$(CC) -shared -o $@.new $(OBJS_ENTITY_DK) -fPIC $(FLAGS_DK) $(INC_DK)
	$(MV) $@.new $@

-include $(DEPS_MC)
-include $(DEPS_DK)

$(OBJDIR_MC)/%.o: %.c
	@$(MKDIRP) $(dir $@)
	$(CC) $(FLAGS_MC) $(INC_MC) -MM -MT $@ -MF $(patsubst %.o,%.d,$@) $<
	$(CC) -o $@ -c -fPIC $(FLAGS_MC) $(INC_MC) $<

$(OBJDIR_DK)/%.o: %.c
	@$(MKDIRP) $(dir $@)
	$(CC) $(FLAGS_DK) $(INC_DK) -MM -MT $@ -MF $(patsubst %.o,%.d,$@) $<
	$(CC) -o $@ -c -fPIC $(FLAGS_DK) $(INC_DK) $<

.rc.o:
	$(WINDRES) $^ -o $@
%.o : %.rc
	$(WINDRES) $^ -o $@

clean:
	-$(RM) $(OBJS_MC) $(OBJS_DK) $(OBJSRES_MC) $(OBJSRES_DK)

mrproper: clean
	-$(RM) -r $(EXE_MC) $(EXE_DK) $(DLL_MC) $(DLL_DK) $(OBJDIR_MC) $(OBJDIR_DK)

distclean: mrproper
