CC = gcc 
CFLAGS = -c -Wall -Werror
LDFLAGS =
SOURCES = archy.c
OBJECTS = $(SOURCES:.cpp=.o)
EXECUTABLE = archy

all: $(SOURCES) $(EXECUTABLE)
	
$(EXECUTABLE): $(OBJECTS) 
	$(CC) $(LDFLAGS) $(OBJECTS) -o $@

.cpp.o:
	$(CC) $(CFLAGS) $< -o $@

.PHONY: clean
clean:
	@rm -f *.o *.s $(EXECUTABLE)
