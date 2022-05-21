CC = g++ 
FLAGS = -g -no-pie

build: code_gen.o SymStack.o

code_gen.o: code_gen.h code_gen.cpp ../ast/ast.h
	$(CC) -g -c code_gen.cpp

SymStack.o: SymStack.h SymStack.cpp ../ast/ast.h
	$(CC) -g -c SymStack.cpp

clean:
	# rm -f code_gen.o SymStack.o test test.ll test.bc exe.s exe 
	rm -f *.o