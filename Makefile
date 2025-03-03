# Compiler and flags
CC = gcc
CFLAGS = -Wall -Werror -g -O2 -fPIC
AR = ar
ARFLAGS = rcs
RM = rm -f

# Directories
SRC_DIR = src
OBJ_DIR = build/obj
INCLUDE_DIR = include
LIB_DIR = build

# Library name
LIBRARY = $(LIB_DIR)/libdynmem.a

# Find all C source files in the src directory
SRC_FILES = $(wildcard $(SRC_DIR)/*.c)

# Create object file paths by replacing .c with .o and placing them in obj directory
OBJ_FILES = $(SRC_FILES:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)

# Targets
all: $(LIBRARY)

# Create static library from object files
$(LIBRARY): $(OBJ_FILES)
	$(AR) $(ARFLAGS) $@ $^

# Compile source files into object files
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(OBJ_DIR)  # Create the obj directory if it doesn't exist
	$(CC) $(CFLAGS) -I$(INCLUDE_DIR) -c $< -o $@

# Clean up generated files
clean:
	$(RM) $(OBJ_DIR)/*.o $(LIBRARY)

# Rebuild the library
rebuild: clean all

.PHONY: all clean rebuild

