CXX := g++-7

CXXFLAGS := -std=c++17 -O2 -g -Wall -Wno-sign-compare
CPPFLAGS :=
LD_FLAGS :=

SRCS := dif.cc streamsplitter.cc main.cc
OBJS := $(SRCS:.cc=.o)

all: dif

dif: $(OBJS)
	$(CXX) $(OBJS) $(CXXFLAGS) $(LD_FLAGS) -o dif

test: dif_test.o
	$(CXX) dif_test.o $(CXXFLAGS) $(LD_FLAGS) -o test

%.o: %.cc
	$(CXX) -c $(CPPFLAGS) $(CXXFLAGS) $< -o $@
	$(CXX) -MM $(CPPFLAGS) $(CXXFLAGS) $< -MT $@ > $*.d


clean:
	rm -rf $(OBJS) dif_test.o

distclean: clean
	rm -rf dif