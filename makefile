# Makefile for bin/test
# created with makefile-creator


####################
#Settings:


CC =  clang++
CFLAGS =  -Wall -Wextra -pedantic -O3 -std=c++11 -DDLO2_USE_POSIX -O3 -Werror -Wextra -pedantic -std=c++11 -mtune=native -march=native 
CLIBS = 
INCLUDES = 
TARGET = bin/test
OBJECTS = build/test.o


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


build/test.o: src/test.cpp src/format.hpp 

