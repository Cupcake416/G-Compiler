CC = g++
LLVM_CONFIG = `llvm-config --system-libs --cppflags --ldflags --libs core`

visualization.o: visualization.h ast.h
	$(CC) -g -c visualization.cpp $(LLVM_CONFIG)

clean:
	rm -f *.o *.png