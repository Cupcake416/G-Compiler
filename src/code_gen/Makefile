CC = g++ -no-pie

test.ll: test
	./test > test.ll

# gcc serves as assembler 
exe: exe.s
	$(CC) -o exe exe.s

exe.s: test.ll
	llvm-as test.ll -o test.bc
	llc test.bc -o exe.s

test: test.cpp code_gen.o SymStack.o
	$(CC) -g -o test test.cpp code_gen.o SymStack.o `/cplab/llvm-project/build/bin/llvm-config --cxxflags --ldflags --system-libs --libs core interpreter MCJIT`

code_gen.o: code_gen.h code_gen.cpp ../ast/ast.h
	$(CC) -g -c code_gen.cpp

SymStack.o: SymStack.h SymStack.cpp ../ast/ast.h
	$(CC) -g -c SymStack.cpp

clean:
	rm -f code_gen.o SymStack.o test test.ll test.bc exe.s exe 