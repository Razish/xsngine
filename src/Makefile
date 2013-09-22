CC = g++
ARCH = $(shell uname -m)
TARGET = ../bin/xsn.$(ARCH)

# compiler flags
FLAGS += -g3 -D_DEBUG
#FLAGS += -O2 -DNDEBUG
FLAGS += -I./ -I./freetype/
FLAGS += -Wall
FLAGS += -Wno-char-subscripts
FLAGS += -msse3
FLAGS += -std=c++11

# source files
FILES += XSClient/XSInput.o XSClient/XSKeys.o
FILES += XSCommon/XSColours.o XSCommon/XSConsole.o XSCommon/XSCvar.o XSCommon/XSEvent.o XSCommon/XSString.o
FILES += GLee/GLee.o XSRenderer/XSFont.o XSRenderer/XSFramebuffer.o XSRenderer/XSInternalFormat.o XSRenderer/XSRenderer.o XSRenderer/XSShaderProgram.o XSRenderer/XSTexture.o
FILES += System/XSMain.o

# libraries
LIBS = $(shell sdl2-config --libs) -lGL  -lm -lfreetype

# targets

$(TARGET): $(FILES)
	$(CC) $(FLAGS) -o $(TARGET) $(FILES) $(LIBS)
	chmod +x $(TARGET)

.cpp.o:
	$(CC) $(FLAGS) -c $*.cpp -o $*.o

clean:
	rm -f $(FILES) $(TARGET)
