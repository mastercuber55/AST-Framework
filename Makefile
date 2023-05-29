
CPP      = g++.exe
CC       = gcc.exe
WINDRES  = windres.exe
OBJ      = AST.o main.o Scenes/Scene_Title.o Scenes/Scene_Game.o
LINKOBJ  = $(OBJ)
LIBS     = -L"C:/MinGW64/x86_64-w64-mingw32/lib32" -L"C:/YOUR_SDL2_PATH/SDL2/lib" -static-libgcc -lmingw32 -lSDL2main -lSDL2 -lSDL2_image -m32
INCS     = -I"C:/MinGW64/include" -I"C:/YOUR_SDL2_PATH/SDL2/include/SDL2"
CXXINCS  = $(INCS) -I"C:/MinGW64/x86_64-w64-mingw32/include" -I"C:/MinGW64/lib/gcc/x86_64-w64-mingw32/4.9.2/include" -I"C:/MinGW64/lib/gcc/x86_64-w64-mingw32/4.9.2/include/c++"
BIN      = "YOUR_EXECUTABLE_NAME.exe"
CXXFLAGS = $(CXXINCS) -m32 -std=c++11 -std=c++1y
CFLAGS   = $(INCS) -m32 -std=c++11 -std=c++1y
RM       = rm.exe -f

.PHONY: all all-before all-after clean clean-custom

all: all-before $(BIN) all-after

clean: clean-custom
	${RM} $(OBJ) $(BIN)

$(BIN): $(OBJ)
	$(CPP) $(LINKOBJ) -o $(BIN) $(LIBS)