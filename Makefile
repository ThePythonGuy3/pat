TARGET := pat

ZLIB_SRC_DIR := ./zlib
ZLIB_BUILD_DIR := ./zlib/build

BUILD_DIR := ./build
SRC_DIR := .

COMP_DIRS := $(shell find $(SRC_DIR) -name '*.cpp')
COMP_DIRS := $(subst ./,,$(COMP_DIRS))
FINAL_DIRS := $(COMP_DIRS:%=$(BUILD_DIR)/%.o)

$(TARGET): $(FINAL_DIRS) zlib/libz.a
	@echo Creating executable file...
	@$(CXX) $(FINAL_DIRS) -o $@ -L./zlib -lz -O3 -std=c++20
	@echo Completed.

$(BUILD_DIR)/%.cpp.o: %.cpp
	@echo Compiling $<...
	@mkdir -p $(dir $@)
	@$(CXX) -c $< -o $@ -O3 -std=c++20

zlib/libz.a:
	@echo Generating static library from zlib...
	@$(MAKE) -C zlib
	@echo Completed zlib.

.PHONY: clean
clean:
	@if [ -d $(BUILD_DIR) ]; then \
		rm -r $(BUILD_DIR); \
	fi
	@$(MAKE) -C zlib clean
	@echo Done.
