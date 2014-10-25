# Makefile for ../bin/tests
# created with makefile-creator


####################
#Settings:


CXX ?= g++
FLAGS +=  -Wall -Wextra -Wpedantic -std=c++1y 
LIBS += 
INCLUDES += 
TARGET = ../bin/tests
OBJECTS = ../build/dummy.o ../build/test.o


####################
#Rules:


$(TARGET) : $(OBJECTS)
	$(CXX) $(FLAGS) -o $(TARGET) $(OBJECTS) $(LIBS)

../build/%.o:
	@if test ! -d '../build'; then mkdir '../build'; echo "created '../build'" ; fi
	$(CXX) $(FLAGS) $(INCLUDES) -c -o $@ $<

clean:
	rm ../build/*.o

all: $(TARGET)


####################
#Dependencies:


../build/dummy.o: dummy.cpp makefile

../build/test.o: test.cpp makefile

