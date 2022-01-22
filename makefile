
CC = g++
CFLAGS = -Wall -ansi -std=c++11 -O2
HEADERS = Utility.h Structure.h GlobalVariable.h
# SOURCES = OpExp_main.cpp Utility.cpp Structure.cpp
# SOURCES = SA_main.cpp Utility.cpp Structure.cpp
SOURCES = deterministicGA.cpp dtmUtility.cpp dtmStructure.cpp

all:
# $(CC) $(CFLAGS) $(SOURCES)  -o SAExp.out
# $(CC) $(CFLAGS) $(SOURCES)  -o OpExp.out
	$(CC) $(CFLAGS) $(SOURCES)  -o dtmGA.out

clean:
	rm -f OpExp.out SAExp.out FSAExp.out dtmGA.out