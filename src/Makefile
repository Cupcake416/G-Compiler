CC = g++
FLAGS = -g -O2
LLVM_CONFIG = `llvm-config --system-libs --cppflags --ldflags --libs core MCJIT Interpreter`

compiler: compiler.cpp
	make -C ./parser -s 
	make -C ./code_gen -s
	make -C ./ast -s
	$(CC) $(FLAGS) ./parser/*.o ./ast/*.o ./code_gen/*.o -o compiler compiler.cpp $(LLVM_CONFIG) -std=c++17

test: compiler
	./compiler < $(target) > test.ll
	llvm-as test.ll -o test.bc
	llc test.bc -o exe.s
	$(CC) -no-pie -o exe exe.s	
	dot -Tpng ./ast/ast_vis.dot -o ./ast/ast_vis.png
	
clean:
	make clean -C ./parser -s
	make clean -C ./ast -s
	make clean -C ./code_gen -s
	rm -f compiler
	rm -f test.ll test.bc exe*