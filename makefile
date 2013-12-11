# Makefile for bin/test
# created with makefile-creator


####################
#Settings:


CC = clang++
CFLAGS = -Wall -Wextra -pedantic -std=c++11 -DYOGA_DEBUG -DYOGA_BE_AWESOME -O0 -std=c++11 -D_GLIBCXX_DEBUG -g 
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


build/dummy.o: src/test/dummy.cpp src/include/yoga.hpp 

build/test.o: src/test/test.cpp src/include/yoga.hpp 

