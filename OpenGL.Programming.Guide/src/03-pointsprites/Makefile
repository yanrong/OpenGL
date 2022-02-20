PWD := $(shell pwd)
CC := gcc
CXX := g++

TOPDIR := $(PWD)/../..
LIBDIR := $(TOPDIR)/lib
# C flags may not need for this project
#CFLAGS := -I$(TOPDIR)/include -I$(TOPDIR)
#default disable the debug info output
#CXXFLAGS := -I$(TOPDIR)/include -I$(TOPDIR) -D_DEBUG
CXXFLAGS := -I$(TOPDIR)/include -I$(TOPDIR)
LDFLAGS := -lGL -lGLEW -lglfw -ldl

LIBCXXSOURCE := $(LIBDIR)/LoadShaders.cpp $(LIBDIR)/targa.cpp
CXXSOURCE := $(wildcard *.cpp)
CXXEXE := $(CXXSOURCE:.cpp=)
#generate implicit automatically compile rule
CXXOBJES := $(LIBCXXSOURCE:.cpp=.o)

#LIBCSOURCE := $(wildcard $(LIBDIR)/*.c)
#COBJS := $(LIBCSOURCE:.c=.o)

all: $(CXXEXE)

#define explicit rule compile each cpp file to .o file
#$(LIBDIR)/%.o: %.cpp
#	$(CXX) -c -o $@ $< $(CXXFLAGS)

#we add the .o file to dependency list, make will generate .o by explicit rules
$(CXXEXE): $(CXXSOURCE) $(CXXOBJES)
	$(CXX) -o $@ $^ $(CXXFLAGS) $(LDFLAGS)

.PHONY: clean

clean:
	rm -rf $(CXXEXE) *.o $(LIBDIR)/*.o
