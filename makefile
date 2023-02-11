all: main
	./main
	dot -Tpng dot.dot -o graph.png

main: main.o
	g++ main.o -o main

main.o:	main.cpp class_testing.h
	g++ -c -fno-elide-constructors main.cpp -o main.o
