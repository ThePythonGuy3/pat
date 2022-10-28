CC = gcc

BUILD_DIR := ./build
SRC_DIR := .

COMP_DIRS := $(shell find $(SRC_DIR) -name '*.c')
COMP_DIRS := $(subst ./,,$(COMP_DIRS))
FINAL_DIRS := $(COMP_DIRS:%=$(BUILD_DIR)/%.o)

libz.a: $(FINAL_DIRS)
	@echo Creating libz.a...
	@ar -rc libz.a build/*.o

$(BUILD_DIR)/%.c.o: %.c
	@echo Compiling $<...
	@mkdir -p $(dir $@)
	@$(CC) -c $< -o $@

.PHONY: clean
clean:
	@if [ -d $(BUILD_DIR) ]; then \
		rm -r $(BUILD_DIR); \
	fi

