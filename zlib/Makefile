CC = gcc

BUILD_DIR := ./build
SRC_DIR := .

COMP_DIRS := $(shell find $(SRC_DIR) -name '*.c')
COMP_DIRS := $(subst ./,,$(COMP_DIRS))
FINAL_DIRS := $(COMP_DIRS:%=$(BUILD_DIR)/%.o)

libz.a: $(FINAL_DIRS)
	ar -rc libz.a build/*.o

$(BUILD_DIR)/%.c.o: %.c
	mkdir -p $(dir $@)
	$(CC) -c $< -o $@

.PHONY: clean
clean:
	-rm -r $(BUILD_DIR)

