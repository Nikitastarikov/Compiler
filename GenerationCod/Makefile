all: clean Test1 Stack

Test1: Test1.s
	gcc -m32 -o Test1 Test1.s

Test: Test.s
	gcc -m64 -o Test Test.s

Stack: stack.s
	gcc -m32 -o Stack stack.s

clean:	
	rm -f Test1 Test Stack
