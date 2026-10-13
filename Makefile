CXX = c++
CPPFLAGS = -I.
CXXFLAGS = -std=c++17 -Wall -Wextra -Wpedantic
SOURCES = Src/main.cpp Ui/Menu.cpp
HEADERS = $(wildcard Include/*.hpp BaseStructures/*.hpp Sequences/*.hpp Tests/*.hpp Ui/*.hpp)
TARGET = binary_search_tree
SANITIZER_TARGET = build/sanitize/binary_search_tree
RELEASE_TARGET = build/release/binary_search_tree

.PHONY: all test performance sanitize release clean graphs

all: $(TARGET)

$(TARGET): $(SOURCES) $(HEADERS) Makefile
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -O2 $(SOURCES) -o $@

test: $(TARGET)
	./$(TARGET) --test

$(RELEASE_TARGET): $(SOURCES) $(HEADERS) Makefile
	mkdir -p $(dir $@)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -O2 -DNDEBUG $(SOURCES) -o $@

release: $(RELEASE_TARGET)

PERFORMANCE_ARGS ?=

performance: $(RELEASE_TARGET)
	./$(RELEASE_TARGET) --performance $(PERFORMANCE_ARGS)

CSV ?= docs/performance-avl.csv

graphs:
	python3 Tools/PlotPerformance.py $(CSV) docs/graphs

$(SANITIZER_TARGET): $(SOURCES) $(HEADERS) Makefile
	mkdir -p $(dir $@)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -O1 -g -fno-omit-frame-pointer -fsanitize=address,undefined -fno-sanitize-recover=undefined $(SOURCES) -o $@

sanitize: $(SANITIZER_TARGET)
	./$(SANITIZER_TARGET) --test

clean:
	rm -f $(TARGET) $(SANITIZER_TARGET) $(RELEASE_TARGET)
