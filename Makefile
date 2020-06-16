LO = build/FunctionLex.o
PO = build/FunctionPars.o
SO = build/FunctionSem.o
EO = build/Exm.o
CC = g++ -std=c++11 -o
CFLAGS = -g3

all: clean Compiler

Compiler: build $(LO) $(PO) $(SO) $(EO)
	$(CC) Compiler $(LO) $(PO) $(SO) $(EO) $(CFLAGS) 

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

build:
	mkdir build

.PHONY: clean
clean:
	rm -rf build/*.o Compiler
