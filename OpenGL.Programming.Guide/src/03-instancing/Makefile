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

#$(COBJS) : $(LIBCSOURCE)
#	$(CC) -c -o $@ $< $(LDFLAGS) $(CFLAGS)

all: $(CXXEXE)

#$(CXXOBJES) : $(LIBCXXSOURCE)
#	$(CXX) -c -o $@ $< $(CXXFLAGS)

$(CXXEXE): $(CXXSOURCE) $(LIBCXXSOURCE)
	$(CXX) -o $@ $<  $(LIBCXXSOURCE) $(CXXFLAGS) $(LDFLAGS)

.PHONY: clean

clean:
	rm -rf $(CXXEXE) *.o $(LIBDIR)/*.o
