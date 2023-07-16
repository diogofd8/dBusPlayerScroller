# Compiler information
CC := g++
CFLAGS := -std=c++17 -Wall -Wextra -pedantic -O3 -g -pthread

# Directories
SRCDIR := src
INCDIR := lib
OBJDIR := obj

# Source files
SRCS := $(wildcard $(SRCDIR)/*.cpp)

# Object files
OBJS := $(patsubst $(SRCDIR)/%.cpp,$(OBJDIR)/%.o,$(SRCS))

# Target executable
TARGET := dBusPlayerScroller

# Rules
all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^

$(OBJDIR)/%.o: $(SRCDIR)/%.cpp
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) -I$(INCDIR) -c $< -o $@

clean:
	rm -f $(TARGET)

cleanall:
	rm -f $(OBJS) $(TARGET)
