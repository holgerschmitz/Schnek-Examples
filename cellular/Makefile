
TARGET = cellular

BINDIR = bin

FULLTARGET = $(BINDIR)/$(TARGET)

OBJECTS = src/cellular.o \
	src/neighbourhood.o\
	src/rules.o

HEADERS = src/cellular.hpp \
	src/neighbourhood.hpp \
	src/rules.hpp


LINK_OBJ = ../include/gui/colorplot.o

CXX = mpic++
CXXFLAGS = -g -Wall `pkg-config --cflags gtk+-3.0`

INCLUDES = -I/usr/local/include -I../include
LDFLAGS = $(LINK_OBJ) -L/usr/lib/i386-linux-gnu -lschnek -lhdf5 -lboost_thread -lboost_system `pkg-config --libs gtk+-3.0` -lfreetype

%.o: %.cpp
	$(CXX) -c $(CXXFLAGS) $(INCLUDES) -o $@ $<

all: $(FULLTARGET)

$(FULLTARGET): gui $(HEADERS) $(OBJECTS)
	$(CXX) $(CXXFLAGS) $(OBJECTS) $(LDFLAGS) -o $(FULLTARGET)
	
gui:
	cd ../include/gui && make
	
.PHONY: clean gui

clean:
	rm -f $(OBJECTS) $(FULLTARGET)