# Compiler
CC = g++
# Compiler flags
CFLAGS = -Wall -Iheader -lssl -lcrypto

# Source files
SRCS := $(shell find src -name '*.cpp')

# Object files
OBJS := $(patsubst src/%.cpp,bin/%.o,$(SRCS))

# Target executable
TARGET = bin/enforce_password

.DEFAULT_GOAL := all

# Rule to build the executable
$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^

# Rule to build object files from C source files
bin/%.o: src/%.cpp
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) -c -o $@ $<

all: $(TARGET)

# Clean rule
clean:
	rm -f $(OBJS) $(TARGET)
