# Makefile for a.out
# created with makefile-creator


####################
#Settings:


CXX ?= g++
FLAGS +=  -Wall -Wextra -Wpedantic -std=c++1y -O0 -D_GLIBCXX_DEBUG -g 
LIBS += 
INCLUDES += 
TARGET = a.out
OBJECTS = dummy.o test.o


####################
#Rules:


$(TARGET) : $(OBJECTS)
	$(CXX) $(FLAGS) -o $(TARGET) $(OBJECTS) $(LIBS)

%.o:
	$(CXX) $(FLAGS) $(INCLUDES) -c -o $@ $<

clean:
	rm *.o

all: $(TARGET)


####################
#Dependencies:


dummy.o: dummy.cpp makefile

test.o: test.cpp makefile

