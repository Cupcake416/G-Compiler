CC = clang++
FLAGS = -g
LLVM_CONFIG = `llvm-config --system-libs --cppflags --ldflags --libs core`

build: lexical.o grammar.o

lexical.o: lexical.cpp ../ast/ast.h
	$(CC) $(FLAGS) -c lexical.cpp $(LLVM_CONFIG) -std=c++14

lexical.cpp: lexical.l grammar.cpp
	flex -o lexical.cpp lexical.l

grammar.o: grammar.cpp ../ast/ast.h
	$(CC) $(FLAGS) -c grammar.cpp $(LLVM_CONFIG) -std=c++14

grammar.cpp: grammar.y
	bison -d -o grammar.cpp grammar.y

clean:
	rm -f *.cpp *.o *.hpp