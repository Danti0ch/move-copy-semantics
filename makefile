BUILD_DIR = build
SRC_DIR   = src
INC_DIR   = inc
LOGS_DIR = logs

GRAPH = graph

all: simple_test
	make clear
	clear
	./simple_test
	dot -Tpng dot.dot -o $(LOGS_DIR)/$(GRAPH).png

optimized_swap_test:
	g++ -c -fno-elide-constructors -D ALLOW_COPY_SEMANTICS -D ALLOW_MOVE_SEMANTICS -D IS_OPTIMIZED=1 -I$(INC_DIR) $(SRC_DIR)/swap_test.cpp -o $(BUILD_DIR)/swap_test.o
	g++ $(BUILD_DIR)/swap_test.o -o swap_test
	./swap_test
	dot -Tpng dot.dot -o $(LOGS_DIR)/$(GRAPH).png

non_optimized_swap_test:
	g++ -c -fno-elide-constructors -D ALLOW_COPY_SEMANTICS -D ALLOW_MOVE_SEMANTICS -D IS_OPTIMIZED=0 -I$(INC_DIR) $(SRC_DIR)/swap_test.cpp -o $(BUILD_DIR)/swap_test.o
	g++ $(BUILD_DIR)/swap_test.o -o swap_test
	./swap_test
	dot -Tpng dot.dot -o $(LOGS_DIR)/$(GRAPH).png

simple_test: $(BUILD_DIR)/simple_test.o
	g++ $(BUILD_DIR)/simple_test.o -o simple_test

$(BUILD_DIR)/simple_test.o: $(SRC_DIR)/simple_test.cpp $(INC_DIR)/tracker.h
	g++ -c -fno-elide-constructors -D ALLOW_MOVE_SEMANTICS -D ALLOW_COPY_SEMANTICS -I$(INC_DIR) \$(SRC_DIR)/simple_test.cpp -o $(BUILD_DIR)/simple_test.o

clear:
	rm $(BUILD_DIR)/*

clear_log:
	rm $(LOGS_DIR)/*
