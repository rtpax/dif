CXX := g++

RELEASE_DIR := release
RELEASE_CXX_FLAGS := -std=c++17 -O2 -g -Wall -Wno-sign-compare
RELEASE_CPP_FLAGS := -DNDEBUG
DEBUG_DIR := debug
DEBUG_CXX_FLAGS := -std=c++17 -O0 -fno-inline -g3 -Wall -Wno-sign-compare
DEBUG_CPP_FLAGS := -DDEBUG
PROFILE_DIR := profile
PROFILE_CXX_FLAGS := -std=c++17 -O2 -pg -Wall -Wno-sign-compare
PROFILE_CPP_FLAGS := -DNDEBUG

BUILD_DIR := $(RELEASE_DIR)
CXXFLAGS := $(RELEASE_CXX_FLAGS)
CPPFLAGS := $(RELEASE_CPP_FLAGS)

ifeq ($(config), release)
BUILD_DIR := $(RELEASE_DIR)
CXXFLAGS := $(RELEASE_CXX_FLAGS)
CPPFLAGS := $(RELEASE_CPP_FLAGS)
endif
ifeq ($(config), debug)
BUILD_DIR := $(DEBUG_DIR)
CXXFLAGS := $(DEBUG_CXX_FLAGS)
CPPFLAGS := $(DEBUG_CPP_FLAGS)
endif
ifeq ($(config), profile)
BUILD_DIR := $(PROFILE_DIR)
CXXFLAGS := $(PROFILE_CXX_FLAGS)
CPPFLAGS := $(PROFILE_CPP_FLAGS)
endif

INSTALL_DIR := ~/bin

LD_LIBS :=

T_SRCS := main_test.cc
T_OBJS := $(T_SRCS:%.cc=$(BUILD_DIR)/%.o)
T_DEPS := $(T_SRCS:%.cc=$(BUILD_DIR)/%.d)

SRCS := dif.cc streamsplitter.cc dif_ostream.cc main.cc
OBJS := $(SRCS:%.cc=$(BUILD_DIR)/%.o)
DEPS := $(SRCS:%.cc=$(BUILD_DIR)/%.d)

.PHONY: clean distclean all dif test

dif: $(BUILD_DIR) $(BUILD_DIR)/dif

test: $(BUILD_DIR) $(BUILD_DIR)/test

all: dif test

install:
	cp $(BUILD_DIR)/dif $(INSTALL_DIR)

$(BUILD_DIR):
	mkdir $(BUILD_DIR)

$(BUILD_DIR)/dif: $(BUILD_DIR) $(OBJS)
	$(CXX) $(OBJS) $(CXXFLAGS) $(LD_LIBS) -o $(BUILD_DIR)/dif

$(BUILD_DIR)/test: $(BUILD_DIR) $(T_OBJS)
	$(CXX) $(T_OBJS) $(CXXFLAGS) $(LD_LIBS) -o $(BUILD_DIR)/test

$(BUILD_DIR)/%.o: %.cc
	$(CXX) -c $(CPPFLAGS) $(CXXFLAGS) $< -o $@
	$(CXX) -MM $(CPPFLAGS) $(CXXFLAGS) $< -MT $@ > $(BUILD_DIR)/$*.d

-include $(DEPS) $(T_DEPS)

clean-objs:
	rm -rf $(OBJS) $(T_OBJS)
	rm -rf $(DEPS) $(T_DEPS)

clean:
	rm -rf $(BUILD_DIR)

clean-all:
	rm -rf $(RELEASE_DIR)
	rm -rf $(DEBUG_DIR)
	rm -rf $(PROFILE_DIR)
	