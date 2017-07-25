# Variables
CC = gcc
CC_FLAGS = -w

# File names
EXEC = main
SOURCES = $(wildcard *.c)
OBJECTS = $(SOURCES:.c=.o)

# Main target
$(EXEC): $(OBJECTS)
	$(CC) $(OBJECTS) -o $(EXEC)

# object files
%.o: %.c
	$(CC) -c $(CC_FLAGS) $< -o $@

clean:
	rm -f $(EXEC) $(OBJECTS)
	
