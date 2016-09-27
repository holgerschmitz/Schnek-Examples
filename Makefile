
TARGET = cellular

BINDIR = bin

FULLTARGET = $(BINDIR)/$(TARGET)

OBJECTS = src/cellular.o \
	src/neighbourhood.o\
	src/rules.o


CXX = mpic++
CXXFLAGS = -g -O3 -Wall

INCLUDES = -I/usr/local/include
LDFLAGS = -lschnek -L/usr/lib/x86_64-linux-gnu/ -lhdf5

%.o: %.cpp
	$(CXX) -c $(CXXFLAGS) $(INCLUDES) -o $@ $<
	
all: $(FULLTARGET)

$(FULLTARGET): $(OBJECTS)
	$(CXX) $(CXXFLAGS) $(OBJECTS) $(LDFLAGS) -o $(FULLTARGET)

clean:
	rm -f $(OBJECTS) $(FULLTARGET)