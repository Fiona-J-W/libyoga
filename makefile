# Makefile for bin/test
# created with makefile-creator


####################
#Settings:


CC = clang++
CFLAGS = -Wall -Wextra -pedantic -std=c++11 -O3 -Werror -mtune=native -march=native 
CLIBS = 
INCLUDES = 
TARGET = bin/test
OBJECTS = build/print.o build/dummy.o build/test.o


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


build/print.o: src/include/print.cpp src/include/format.hpp src/include/print.hpp src/include/util.hpp 

build/dummy.o: src/test/dummy.cpp src/include/format.hpp src/include/macros.hpp src/include/print.hpp src/include/util.hpp src/include/yoga.hpp 

build/test.o: src/test/test.cpp src/include/format.hpp src/include/macros.hpp src/include/print.hpp src/include/util.hpp src/include/yoga.hpp 

