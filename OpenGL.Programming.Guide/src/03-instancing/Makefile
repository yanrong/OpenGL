PWD := $(shell pwd)
CC := gcc
CXX := g++

TOPDIR := $(PWD)/../..
LIBDIR := $(TOPDIR)/lib
#CFLAGS := -I$(TOPDIR)/include -I$(TOPDIR)
CXXFLAGS := -I$(TOPDIR)/include -I$(TOPDIR)
LDFLAGS := -lGL -lGLEW -lglfw -ldl

LIBCXXSOURCE := $(LIBDIR)/LoadShaders.cpp $(LIBDIR)/vbm.cpp
CXXSOURCE := $(wildcard *.cpp)
CXXEXE := $(CXXSOURCE:.cpp=)
CXXOBJES := $(LIBCXXSOURCE:.cpp=.o)

#LIBCSOURCE := $(wildcard $(LIBDIR)/*.c)
#COBJS := $(LIBCSOURCE:.c=.o)

all: $(CXXEXE)

#define explicit rule compile each cpp file to .o file
#$(LIBDIR)/%.o: %.cpp
#	$(CXX) -c -o $@ $< $(CXXFLAGS)

#we add the .o file to dependency list, make will automatically generate .o by implicit rules
$(CXXEXE): $(CXXSOURCE) $(CXXOBJES)
	$(CXX) -o $@ $^ $(CXXFLAGS) $(LDFLAGS)

.PHONY: clean

clean:
	rm -rf $(CXXEXE) *.o $(LIBDIR)/*.o
