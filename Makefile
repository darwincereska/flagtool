# Compiler and flags
CC = gcc
CFLAGS = -Wall -Wextra -std=c17 -Iinclude

# Library source and objects
SRC = src/flagtool.c
OBJ = $(SRC:.c=.o)
LIB = libflagtool.a

# Test source, objects, and binary
TEST_SRC = tests/test_flagtool.c
TEST_OBJ = $(TEST_SRC:.c=.o)
TEST_BIN = test_flagtool

# Example source, objects, and binary
EXAMPLE_SRC = examples/example.c
EXAMPLE_OBJ = $(EXAMPLE_SRC:.c=.o)
EXAMPLE_BIN = example

.PHONY: all clean test example lib

# Default: build library, test, and example executables
all: $(LIB) test example

# Build static library from object files
$(LIB): $(OBJ)
	ar rcs $@ $^
lib: $(LIB)

# Build test executable
test: $(LIB) $(TEST_OBJ)
	$(CC) $(CFLAGS) -o $(TEST_BIN) $(TEST_OBJ) $(LIB)

# Build example executable
example: $(LIB) $(EXAMPLE_OBJ)
	$(CC) $(CFLAGS) -o $(EXAMPLE_BIN) $(EXAMPLE_OBJ) $(LIB)

# Compile .c files into .o object files
%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

# Clean build files
clean:
	rm -f $(OBJ) $(TEST_OBJ) $(EXAMPLE_OBJ) $(LIB) $(TEST_BIN) $(EXAMPLE_BIN)
