CXX := g++

BUILD_DIR := build

CXXFLAGS := -std=c++17 -O2 -g -Wall -Wno-sign-compare
CPPFLAGS :=
LD_FLAGS :=


T_SRCS := main_test.cc
T_OBJS := $(T_SRCS:.cc=.o)
T_DEPS := $(T_DEPS:.cc=.d)

SRCS := dif.cc streamsplitter.cc dif_ostream.cc main.cc
OBJS := $(SRCS:.cc=.o)
DEPS := $(SRCS:.cc=.o)


all: build dif

build:
	mkdir build

dif: $(OBJS)
	$(CXX) $(OBJS) $(CXXFLAGS) $(LD_FLAGS) -o $(BUILD_DIR)/dif

catch: build $(T_OBJS)
	$(CXX) $(T_OBJS) $(CXXFLAGS) $(LD_FLAGS) -o $(BUILD_DIR)/test

%.o: %.cc
	$(CXX) -c $(CPPFLAGS) $(CXXFLAGS) $< -o $@
	$(CXX) -MM $(CPPFLAGS) $(CXXFLAGS) $< -MT $@ > $*.d

-include $(DEPS) dif_test.d

clean:
	rm -rf $(OBJS) $(T_OBJS)
	rm -rf $(DEPS) $(T_DEPS)

distclean: clean
	rm -rf $(BUILD_DIR)/dif $(BUILD_DIR)/test