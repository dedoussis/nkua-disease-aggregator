PROGRAM=diseaseAggregator
CXXFLAGS=-Wall -Werror -pedantic -std=c++20
OPTIMIZATION=-O2
TARGET_DIR=$(shell pwd)/bin

.PHONY: build clean distrib run docker-run

build: clean $(PROGRAM)

clean:
	rm -rf $(TARGET_DIR)

distrib:
	tar -czf $(PROGRAM).tar.gz *.cpp *.h Makefile

$(PROGRAM): $(PROGRAM).cpp
	mkdir $(TARGET_DIR)
	g++ *.cpp *.h $(CXXFLAGS) $(OPTIMIZATION) -o $(TARGET_DIR)/$(PROGRAM)

run: build
	$(TARGET_DIR)/$(PROGRAM) -w 3 -b 2 -i input_dir

docker-run:
	docker run -it -v "$(shell pwd):/app/" -w /app/ gcc make run
