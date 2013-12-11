# Makefile for bin/test
# created with makefile-creator


####################
#Settings:


CC =  clang++
CFLAGS =  -Wall -Wextra -pedantic -std=c++11  -DYOGA_USE_POSIX -DYOGA_USE_DEBUGGING_UTILITIES -DYOGA_DEBUG 
CLIBS = 
INCLUDES = 
TARGET = bin/test
OBJECTS = build/dummy.o build/test.o


####################
#Rules:


$(TARGET) : $(OBJECTS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJECTS) $(CLIBS)

build/%.o:
	@if test ! -d 'build'; then mkdir 'build'; echo "created 'build'" ; fi
	$(CC) $(CFLAGS) $(INCLUDES) -c -o $@ $<

clean:
	rm build/*.o

all: $(TARGET)


####################
#Dependencies:


build/dummy.o: src/dummy.cpp src/yoga.hpp 

build/test.o: src/test.cpp src/yoga.hpp 

