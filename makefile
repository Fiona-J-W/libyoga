# Makefile for lib/libyoga.so
# created with makefile-creator


####################
#Settings:


CXX = g++
FLAGS +=  -Wall -Wextra -pedantic -std=c++1y -shared -fPIC -O3 -mtune=native -Werror 
LIBS += 
INCLUDES += 
TARGET = lib/libyoga.so
OBJECTS = build/print.o


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


build/print.o: src/lib/print.cpp src/include/format.hpp src/include/print.hpp src/include/util.hpp makefile

