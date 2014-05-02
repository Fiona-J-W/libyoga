# Makefile for bin/test
# created with makefile-creator


####################
#Settings:


CXX ?= g++
FLAGS +=  -Wall -Wextra -pedantic -std=c++11 -O3 -mtune=native -Werror 
LIBS += 
INCLUDES = 
TARGET = bin/test
OBJECTS = build/print.o build/dummy.o build/test.o


####################
#Rules:


$(TARGET) : $(OBJECTS)
	$(CXX) $(FLAGS) -o $(TARGET) $(OBJECTS) $(LIBS)

build/%.o:
	@if test ! -d 'build'; then mkdir 'build'; echo "created 'build'" ; fi
	$(CXX) $(FLAGS) $(INCLUDES) -c -o $@ $<

clean:
	rm build/*.o

all: $(TARGET)


####################
#Dependencies:


build/print.o: src/include/print.cpp src/include/format.hpp src/include/print.hpp src/include/util.hpp makefile

build/dummy.o: src/test/dummy.cpp src/include/format.hpp src/include/macros.hpp src/include/print.hpp src/include/util.hpp src/include/yoga.hpp makefile

build/test.o: src/test/test.cpp src/include/format.hpp src/include/macros.hpp src/include/print.hpp src/include/util.hpp src/include/yoga.hpp makefile

