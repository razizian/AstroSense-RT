# Compiler configuration
CXX = clang++
CXXFLAGS = -std=c++17 -Wall -Wextra -O2 -I./include
LDFLAGS = -pthread

# Directory structure
SRC_DIR = src
OBJ_DIR = obj
BIN_DIR = bin

# File lists
SRCS = $(wildcard $(SRC_DIR)/*.cpp)
OBJS = $(patsubst $(SRC_DIR)/%.cpp,$(OBJ_DIR)/%.o,$(SRCS))
TARGET = $(BIN_DIR)/sensor_processor

# Default target: create directories and build the application
all: directories $(TARGET)

# Create necessary directories for build artifacts
directories:
	mkdir -p $(OBJ_DIR) $(BIN_DIR)

# Link object files to create the executable
$(TARGET): $(OBJS)
	$(CXX) $(OBJS) $(LDFLAGS) -o $@

# Compile source files into object files
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp | directories
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Clean target: remove all build artifacts
clean:
	rm -rf $(OBJ_DIR) $(BIN_DIR)

# Declare phony targets
.PHONY: all clean directories