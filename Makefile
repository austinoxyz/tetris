# rlchess/Makefile

CC         := gcc
CC_VERSION := 17

THIRDPARTY_LIBS := raylib

WARNING_FLAGS := -Wall -Wextra -Werror

CFLAGS := -std=c$(CC_VERSION) -ggdb $(WARNING_FLAGS)
CFLAGS += -Wuninitialized -Wformat

INCLUDES := -I./src/ `pkg-config --cflags $(THIRDPARTY_LIBS)`
SOURCES  := $(wildcard src/*.c) $(wildcard src/**/*.c)
OBJECTS  := $(SOURCES:.c=.o)
LDFLAGS  := `pkg-config --libs $(THIRDPARTY_LIBS)`

DEBUG_DIR := debug
BUILD_DIR := build

BIN_NAME := $(BUILD_DIR)/tetris
TEST_BIN_NAME := $(BUILD_DIR)/tetris

CPPCHECK             := cppcheck
CPPCHECK_OUTFILE     := $(DEBUG_DIR)/cppcheck.out.txt
CPPCHECK_OUTFILE_XML := $(DEBUG_DIR)/cppcheck.out.xml

CLANGTIDY_VERSION := 13
CLANGTIDY         := clang-tidy-$(CLANGTIDY_VERSION)
CLANGTIDY_CHECKS  := clang-analyzer-core.*,clang-analyzer-cplusplus,clang-analyzer-unix.*
CLANGTIDY_OUTFILE := $(DEBUG_DIR)/clang-tidy.out.txt

VALGRIND         := valgrind
VALGRIND_OUTFILE := $(DEBUG_DIR)/valgrind-leaks.out.txt

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
dirs: build-dir debug-dir

.PHONY: build-dir
build-dir:
	mkdir -p ./$(BUILD_DIR)

.PHONY: debug-dir
debug-dir:
	mkdir -p ./$(DEBUG_DIR)
# -----------------------------------

# Thirdparty Libraries
# -----------------------------------
.PHONY: libs
libs: $(THIRDPARTY_LIBS)

.PHONY: raylib
raylib:
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
	gdb $(BIN_NAME)

.PHONY: debug-go
debug-go: main
	gdb -ex run $(BIN_NAME)


.PHONY: cppcheck
cppcheck: debug-dir
	@$(CPPCHECK) --std=c++$(CC_VERSION)                   \
	--quiet --enable=all --error-exitcode=1 --inline-suppr \
	--output-file=$(CPPCHECK_OUTFILE)                      \
	$(INCLUDES) main.c $(SOURCES)
	nvim $(CPPCHECK_OUTFILE)

.PHONY: cppcheck-xml
cppcheck-xml: debug-dir
	@$(CPPCHECK) --std=c++$(CC_VERSION)                   \
	--quiet --enable=all --error-exitcode=1 --inline-suppr \
	--xml-version=2 --output-file=$(CPPCHECK_OUTFILE_XML)  \
	$(INCLUDES) main.c $(SOURCES)
	nvim $(CPPCHECK_OUTFILE_XML)

.PHONY: clang-tidy
clang-tidy: debug-dir
	$(CLANGTIDY) --quiet main.c $(SOURCES) -- \
	-Iinclude -std=c++$(CC_VERSION)

.PHONY: valgrind
valgrind: debug-dir
	$(VALGRIND) --leak-check=full --show-leak-kinds=all --track-origins=yes \
	--log-file=$(VALGRIND_OUTFILE) $(BIN_NAME)
# -----------------------------------

# Testing
# -----------------------------------
.PHONY: test-piece-encoding
test-piece-encoding: all
	$(CC) $(CFLAGS) -o $(TEST_DIR)/piece-encoding/main.bin $(TEST_DIR)/piece-encoding/main.c $(INCLUDES) $(LDFLAGS)
	./$(TEST_DIR)/piece-encoding/main.bin
# -----------------------------------

