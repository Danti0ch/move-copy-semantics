BUILD_DIR = build
SRC_DIR   = src
INC_DIR   = inc
LOGS_DIR = logs

GRAPH = graph
all: main
	make clear
	clear
	./main
	dot -Tpng dot.dot -o $(LOGS_DIR)/$(GRAPH).png

main: $(BUILD_DIR)/main.o
	g++ $(BUILD_DIR)/main.o -o main

$(BUILD_DIR)/main.o: $(SRC_DIR)/main.cpp $(INC_DIR)/tracker.h
	g++ -c -fno-elide-constructors -D ALLOW_COPY_SEMANTICS -D ALLOW_MOVE_SEMANTICS -I$(INC_DIR) \$(SRC_DIR)/main.cpp -o $(BUILD_DIR)/main.o

clear:
	rm $(BUILD_DIR)/*

clear_log:
	rm $(LOGS_DIR)/*
