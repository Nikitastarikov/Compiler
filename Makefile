LO = build/FunctionLex.o
PO = build/FunctionPars.o
SO = build/FunctionSem.o
GCO = build/GenerationCod.o
EO = build/Exm.o
CC = g++ -std=c++11 -o
CFLAGS = -g3
ARGS ?= default_args

all: clean Compiler

Compiler: build $(LO) $(PO) $(SO) $(GCO) $(EO)
	$(CC) Compiler $(LO) $(PO) $(SO) $(GCO) $(EO) $(CFLAGS) 

ignore: 
	mkdir ignore

build/Exm.o: Exm.cpp
	$(CC) $(EO) -c Exm.cpp $(CFLAGS)

build/FunctionLex.o: Lexer/FunctionLex.cpp
	$(CC) $(LO) -c Lexer/FunctionLex.cpp $(CFLAGS)

build/FunctionPars.o: Parser/FunctionPars.cpp
	$(CC) $(PO) -c Parser/FunctionPars.cpp $(CFLAGS)

build/FunctionSem.o: Semantic/FunctionSem.cpp
	$(CC) $(SO) -c Semantic/FunctionSem.cpp $(CFLAGS)

build/GenerationCod.o: GenerationCod/GenerationCod.cpp
	$(CC) $(GCO) -c GenerationCod/GenerationCod.cpp $(CFLAGS)

run: ccomp scomp

build:
	mkdir build

ccomp:
	./Compiler $(ARGS)

scomp:
	gcc -m32 -o proga proga.s

.PHONY: run clean
clean:
	rm -rf build/*.o Compiler proga.s proga
