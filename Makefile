CXX=g++
CXXFLAGS=-Wall -Werror -pedantic -std=c++20
OPTIMIZATION=-O2
PROGRAM=disease-aggregator
TARGET_DIR=bin
SOURCES_DIR=$(PROGRAM)
DEP_DIR=deps
SOURCES=$(wildcard $(SOURCES_DIR)/*.cpp)
OBJECTS=$(patsubst $(SOURCES_DIR)/%.cpp,$(TARGET_DIR)/%.o,$(SOURCES))
DEPFILES=$(patsubst $(SOURCES_DIR)/%.cpp,$(DEP_DIR)/%.d,$(SOURCES))
DEPFLAGS=-MT $@ -MMD -MP -MF $(DEP_DIR)/$*.d

.PHONY: build format distrib run build-docker run-docker build-in-docker

build: $(TARGET_DIR)/$(PROGRAM)

$(TARGET_DIR)/$(PROGRAM): $(OBJECTS) | $(TARGET_DIR)
	$(CXX) $(CXXFLAGS) $(OPTIMIZATION) $^ -o $@

$(TARGET_DIR)/%.o: $(SOURCES_DIR)/%.cpp Makefile $(DEP_DIR)/%.d | $(TARGET_DIR) $(DEP_DIR)
	$(CXX) $(CXXFLAGS) $(OPTIMIZATION) $(DEPFLAGS) -c $< -o $@

$(TARGET_DIR): ; @mkdir -p $@

$(DEP_DIR): ; @mkdir -p $@

$(DEPFILES):

clean:
	rm -rf $(TARGET_DIR) $(DEP_DIR)

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

include $(wildcard $(DEPFILES))
