CC = g++
ARCH = $(shell uname -m)
TARGET = ../bin/xsn.$(ARCH)

# compiler flags
FLAGS += -g3 -D_DEBUG
#FLAGS += -O2 -DNDEBUG
FLAGS += -I./
FLAGS += -Wall
FLAGS += -msse3
FLAGS += -std=c++11

# source files
FILES += System/XSMain.o
FILES += XSClient/XSInput.o XSClient/XSKeys.o XSClient/XSVideo.o
FILES += XSCommon/XSColours.o XSCommon/XSConsole.o XSCommon/XSCvar.o XSCommon/XSEvent.o XSCommon/XSString.o

# libraries
LIBS = $(shell sdl2-config --libs) -lGL  -lm

# targets

$(TARGET): $(FILES)
	$(CC) $(FLAGS) -o $(TARGET) $(FILES) $(LIBS)
	chmod +x $(TARGET)

.cpp.o:
	$(CC) $(FLAGS) -c $*.cpp -o $*.o

clean:
	rm -f $(FILES) $(TARGET)
