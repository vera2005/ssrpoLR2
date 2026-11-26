CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -Wpedantic -Iinclude
COVFLAGS = $(CXXFLAGS) --coverage
LDFLAGS = --coverage

SRC = src
BUILD = build
BIN = bin
TEST = test

LIB_OBJS = $(BUILD)/l2_ApplicationLayer.o $(BUILD)/l3_DomainLayer.o $(BUILD)/l4_InfrastructureLayer.o
UI_OBJ = $(BUILD)/l1_UserInterface.o
TARGET = $(BIN)/airfield

.PHONY: all clean test stress-test
all: $(TARGET)

$(TARGET): $(UI_OBJ) $(LIB_OBJS) | $(BIN)
	$(CXX) $(LDFLAGS) -o $@ $^

$(BUILD)/l1_UserInterface.o: $(SRC)/l1_UserInterface.cpp | $(BUILD)
	$(CXX) $(CXXFLAGS) -c -o $@ $<

$(BUILD)/%.o: $(SRC)/%.cpp | $(BUILD)
	$(CXX) $(COVFLAGS) -c -o $@ $<

$(BIN):
	mkdir -p $(BIN)
$(BUILD):
	mkdir -p $(BUILD)

$(TEST)/test01: $(TEST)/test01.cpp $(LIB_OBJS)
	$(CXX) $(COVFLAGS) -o $@ $^
$(TEST)/test02: $(TEST)/test02.cpp $(LIB_OBJS)
	$(CXX) $(COVFLAGS) -o $@ $^
$(TEST)/stress_test: $(TEST)/stress_test.cpp
	$(CXX) $(CXXFLAGS) -o $@ $^

test: $(TEST)/test01 $(TEST)/test02
	$(TEST)/test01
	$(TEST)/test02

stress-test: $(TEST)/stress_test $(TARGET)
	$(TEST)/stress_test

clean:
	rm -rf $(BUILD) $(BIN) $(TEST)/test01 $(TEST)/test02 $(TEST)/stress_test \
	*.gcda *.gcno t1.data requests.data stress_input.txt stress_output.txt stress_save.data
