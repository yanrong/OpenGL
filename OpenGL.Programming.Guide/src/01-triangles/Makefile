PWD := $(shell pwd)
CC := gcc
CXX := g++

TOPDIR := $(PWD)/../..
LIBDIR := $(TOPDIR)/lib
#CFLAGS := -I$(TOPDIR)/include -I$(TOPDIR)
CXXFLAGS := -I$(TOPDIR)/include -I$(TOPDIR)
LDFLAGS := -lGL -lGLEW -lglfw -ldl

LIBCXXSOURCE := $(wildcard $(LIBDIR)/*.cpp)
CXXSOURCE := $(wildcard *.cpp)
CXXEXE := $(CXXSOURCE:.cpp=)
CXXOBJES := $(LIBCXXSOURCE:.cpp=.o)

all: $(CXXEXE)

$(CXXOBJES) : $(LIBCXXSOURCE)
	$(CXX) -c -o $@ $< $(CXXFLAGS)

$(CXXEXE): $(CXXSOURCE) $(CXXOBJES)
	$(CXX) -o $@ $@.cpp $(CXXOBJES) $(CXXFLAGS) $(LDFLAGS)

.PHONY: clean

clean:
	rm -rf $(CXXEXE) *.o   $(LIBDIR)/*.o