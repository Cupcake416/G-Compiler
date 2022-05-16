test: test.cpp code_gen.o SymStack.o
	g++ -g -o test test.cpp code_gen.o SymStack.o `/cplab/llvm-project/build/bin/llvm-config --cxxflags --ldflags --system-libs --libs core interpreter MCJIT`

code_gen.o: code_gen.h code_gen.cpp ../ast/ast.h

SymStack.o: SymStack.h SymStack.cpp ../ast/ast.h

clean:
	rm code_gen.o SymStack.o test