CXX=g++
CXXFLAGS=-Wall -Werror -pedantic -std=c++20
OPTIMIZATION=-O2
PROGRAM=disease-aggregator
TARGET_DIR=bin
SOURCES_DIR=$(PROGRAM)
SOURCES=$(wildcard $(SOURCES_DIR)/*.cpp)
OBJECTS=$(patsubst $(SOURCES_DIR)/%.cpp,$(TARGET_DIR)/%.o,$(SOURCES))
DEPENDS=$(patsubst $(SOURCES_DIR)/%.cpp,$(TARGET_DIR)/%.d,$(SOURCES))

.PHONY: build format distrib run build-docker run-docker build-in-docker

build: $(TARGET_DIR)/$(PROGRAM)

$(TARGET_DIR)/$(PROGRAM): $(OBJECTS)
	$(CXX) $(CXXFLAGS) $(OPTIMIZATION) $^ -o $@

-include $(DEPENDS)

$(TARGET_DIR)/%.o: $(SOURCES_DIR)/%.cpp Makefile
	$(CXX) $(CXXFLAGS) $(OPTIMIZATION) -MMD -MP -c $< -o $@

clean:
	rm -rf $(TARGET_DIR)

format:
	clang-format -i $(SOURCES_DIR)/**

distrib:
	tar -czf $(PROGRAM).tar.gz $(SOURCES_DIR) Makefile *.sh

run: build
	$(TARGET_DIR)/$(PROGRAM) -w 3 -b 2 -i input_dir.tmp

build-docker:
	docker build -t $(PROGRAM) .

run-docker: build-docker
	docker run --rm -it -v "$(shell pwd):/runner/" $(PROGRAM) -w 3 -b 2 -i /runner/input_dir.tmp

build-in-docker:
	docker run --rm -it -v "$(shell pwd):/app/" -w /app/ gcc make run
