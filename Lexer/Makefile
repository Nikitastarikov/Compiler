all: clean Exm

Exm: FunctionLex.o Exm.o
	g++ -Wall -o Exm FunctionLex.o Exm.o -lm

Exm.o:
	g++ -std=c++11 -c Exm.cpp

FunctionLex.o:
	g++ -std=c++11 -c FunctionLex.cpp

clean:
	rm -f *.o Exm
