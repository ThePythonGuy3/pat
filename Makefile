TARGET := pat

ZLIB_SRC_DIR := ./zlib
ZLIB_BUILD_DIR := ./zlib/build

BUILD_DIR := ./build
SRC_DIR := .

COMP_DIRS := $(shell find $(SRC_DIR) -name '*.cpp')
COMP_DIRS := $(subst ./,,$(COMP_DIRS))
FINAL_DIRS := $(COMP_DIRS:%=$(BUILD_DIR)/%.o)

$(TARGET): $(FINAL_DIRS) zlib/libz.a
	$(CXX) $(FINAL_DIRS) -o $@ -L./zlib -lz -O3 -std=c++20

$(BUILD_DIR)/%.cpp.o: %.cpp
	mkdir -p $(dir $@)
	$(CXX) -c $< -o $@ -O3 -std=c++20

zlib/libz.a:
	$(MAKE) -C zlib

.PHONY: clean
clean:
	-rm -r $(BUILD_DIR)
	$(MAKE) -C zlib clean
