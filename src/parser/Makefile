CC = g++
FLAGS = -g

build: lexical.o grammar.o

lexical.o: lexical.cpp ../ast/ast.h
	$(CC) $(FLAGS) -c lexical.cpp

lexical.cpp: lexical.l grammar.cpp
	flex -o lexical.cpp lexical.l

grammar.o: grammar.cpp ../ast/ast.h
	$(CC) $(FLAGS) -c grammar.cpp

grammar.cpp: grammar.y
	bison -d -o grammar.cpp grammar.y

clean:
	rm -f *.cpp *.o *.hpp