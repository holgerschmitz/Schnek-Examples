
TARGET = gui

BINDIR = bin

FULLTARGET = $(BINDIR)/$(TARGET)

OBJECTS = colorplot.o

HEADERS = colorplot.hpp

CXX = g++
CXXFLAGS = -g -O3 -Wall `pkg-config --cflags gtk+-3.0`

INCLUDES = -I/usr/local/include -I..

%.o: %.cpp
	$(CXX) -c $(CXXFLAGS) $(INCLUDES) -o $@ $<
	
all: $(FULLTARGET)


$(FULLTARGET): $(HEADERS) $(OBJECTS)

clean:
	rm -f $(OBJECTS) $(FULLTARGET)