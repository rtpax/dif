CXX := g++

BUILD_DIR := build
INSTALL_DIR := ~/bin

CXXFLAGS := -std=c++17 -O2 -g -Wall -Wno-sign-compare
CPPFLAGS :=
LD_FLAGS :=


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
	$(CXX) $(OBJS) $(CXXFLAGS) $(LD_FLAGS) -o $(BUILD_DIR)/dif

$(BUILD_DIR)/test: $(BUILD_DIR) $(T_OBJS)
	$(CXX) $(T_OBJS) $(CXXFLAGS) $(LD_FLAGS) -o $(BUILD_DIR)/test

$(BUILD_DIR)/%.o: %.cc
	$(CXX) -c $(CPPFLAGS) $(CXXFLAGS) $< -o $@
	$(CXX) -MM $(CPPFLAGS) $(CXXFLAGS) $< -MT $@ > $(BUILD_DIR)/$*.d

-include $(DEPS) $(T_DEPS)

clean:
	rm -rf $(OBJS) $(T_OBJS)
	rm -rf $(DEPS) $(T_DEPS)

distclean:
	rm -rf $(BUILD_DIR)