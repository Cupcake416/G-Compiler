CC = g++
FALGS = -g 

compiler: compiler.cpp
	make -C ./parser -s
	# make -C ./ast -s
	make -C ./code_gen -s
	# $(CC) $(FLAGS) ./parser/*.o ./ast/*.o ./code_gen/*.o -o compiler compiler.cpp
	$(CC) $(FLAGS) ./parser/*.o ./code_gen/*.o -o compiler compiler.cpp `/cplab/llvm-project/build/bin/llvm-config --cxxflags --ldflags --system-libs --libs core interpreter MCJIT`

test: compiler
	./compiler < $(target) > test.ll
	llvm-as test.ll -o test.bc
	llc test.bc -o exe.s
	$(CC) -no-pie -o exe exe.s	

clean:
	make clean -C ./parser -s
	# make clean -C ./ast -s
	make clean -C ./code_gen -s
	rm -f compiler
	rm -f test.ll test.bc exe*