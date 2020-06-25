LO = build/FunctionLex.o
PO = build/FunctionPars.o
SO = build/FunctionSem.o
GCO = build/GenerationCod.o
EO = build/Exm.o
CC = g++ -std=c++11 -o
BUILD_TESTE_LEXER = build/TestLex.o
BUILD_TESTE_PARSER = build/TestPars.o
BUILD_TESTE_SEMANTIC = build/TestSem.o
BUILD_TESTE_CODGEN = build/TestCodGen.o
CFLAGS = -g3
ARGS ?= default_args

all: clean Compiler TestLexer TestParser TestSemantic TestCodGen

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

TestLexer: $(BUILD_TESTE_LEXER) $(LO)
	$(CC) TestLexer $(BUILD_TESTE_LEXER) $(LO)

build/TestLex.o: Tests/TestLex.cpp
	$(CC) $(BUILD_TESTE_LEXER) -c Tests/TestLex.cpp

TestParser: $(BUILD_TESTE_PARSER) $(LO) $(PO)
	$(CC) TestParser $(BUILD_TESTE_PARSER) $(LO) $(PO)

build/TestPars.o: Tests/TestPars.cpp
	$(CC) $(BUILD_TESTE_PARSER) -c Tests/TestPars.cpp

TestSemantic: $(BUILD_TESTE_SEMANTIC) $(LO) $(PO) $(SO)
	$(CC) TestSemantic $(BUILD_TESTE_SEMANTIC) $(LO) $(PO) $(SO)

build/TestSem.o: Tests/TestSem.cpp
	$(CC) $(BUILD_TESTE_SEMANTIC) -c Tests/TestSem.cpp

TestCodGen: $(BUILD_TESTE_CODGEN) $(LO) $(PO) $(SO) $(GCO)
	$(CC) TestCodGen $(BUILD_TESTE_CODGEN) $(LO) $(PO) $(SO) $(GCO)

build/TestCodGen.o: Tests/TestSem.cpp
	$(CC) $(BUILD_TESTE_CODGEN) -c Tests/TestCodGen.cpp


run: ccomp scomp

build:
	mkdir build

ccomp:
	./Compiler $(ARGS)

scomp:
	gcc -m32 -o proga proga.s

.PHONY: run clean
clean:
	rm -rf build/*.o Compiler proga.s proga TestLexer TestParser TestSemantic TestCodGen
