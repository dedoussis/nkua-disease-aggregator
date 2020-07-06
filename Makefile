PROGRAM=diseaseAggregator
CXXFLAGS=-Wall -Werror -pedantic -std=c++17
OPTIMIZATION=-O2
TARGET_DIR=$(shell pwd)/bin

.PHONY: build clean distrib run

build: clean $(PROGRAM)

clean:
	rm -rf $(TARGET_DIR)

distrib:
	tar -czf $(PROGRAM).tar.gz $(PROGRAM).cpp Makefile

$(PROGRAM): $(PROGRAM).cpp
	mkdir $(TARGET_DIR)
	g++ $(PROGRAM).cpp $(CXXFLAGS) $(OPTIMIZATION) -o $(TARGET_DIR)/$(PROGRAM)

run: build
	$(TARGET_DIR)/$(PROGRAM)
