# rlchess/Makefile

CC         := gcc
CC_VERSION := 17

PROJNAME := tetris

THIRDPARTY_LIBS := raylib

WARNING_FLAGS := -Wall -Wextra -Werror
WARNING_FLAGS += -Wshadow -Wuninitialized -Wformat

CFLAGS := -std=c$(CC_VERSION) -ggdb $(WARNING_FLAGS)

INCLUDES := -I./src/ `pkg-config --cflags $(THIRDPARTY_LIBS)`
SOURCES  := $(wildcard src/*.c)
OBJECTS  := $(SOURCES:.c=.o)
LDFLAGS  := `pkg-config --libs $(THIRDPARTY_LIBS)`

BUILD_DIR := build

BIN_NAME := $(BUILD_DIR)/$(PROJNAME)

# Main interface
# -----------------------------------
.PHONY: run
run: all
	$(BIN_NAME)

.PHONY: all
all: dirs libs main 

.PHONY: clean
clean:
	rm -rf $(BUILD_DIR) $(OBJECTS)
# -----------------------------------

# Directories
# -----------------------------------
.PHONY: dirs
dirs: build-dir

.PHONY: build-dir
build-dir:
	mkdir -p ./$(BUILD_DIR)
# -----------------------------------

# Thirdparty Libraries
# -----------------------------------
# TODO: Provide raylib and build from source at game's build time
.PHONY: libs
libs: # $(THIRDPARTY_LIBS)
	# UNIMPLEMENTED

# .PHONY: raylib
# raylib:
# -----------------------------------

# Build Step
# -----------------------------------
.PHONY: main
main: $(OBJECTS)
	$(CC) $(CFLAGS) main.c -o $(BIN_NAME) $^ $(INCLUDES) $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -o $@ -c $< $(INCLUDES)
# -----------------------------------

# Debugging
# -----------------------------------
.PHONY: debug
debug: main
	gdb --tui -ex run $(BIN_NAME)
# -----------------------------------

# Testing
# -----------------------------------
# -----------------------------------

