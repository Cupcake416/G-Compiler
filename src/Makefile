CC = g++
FALGS = -g 

build: compiler.cpp
	make -C ./parser -s
	# make -C ./ast -s
	make -C ./code_gen -s
	# $(CC) $(FLAGS) ./parser/*.o ./ast/*.o ./code_gen/*.o -o compiler compiler.cpp
	$(CC) $(FLAGS) ./parser/*.o ./code_gen/*.o -o compiler compiler.cpp `/cplab/llvm-project/build/bin/llvm-config --cxxflags --ldflags --system-libs --libs core interpreter MCJIT`

test: compiler test.cpp
	./compiler < ./test.cpp > test.ll
	llvm-as test.ll -o test.bc
	llc test.bc -o exe.s
	$(CC) -no-pie -o exe exe.s	

clean:
	make clean -C ./parser -s
	# make clean -C ./ast -s
	make clean -C ./code_gen -s
	rm compiler
	rm test.ll test.bc exe*