# Use Ubuntu 22.04 LTS as the base image
FROM ubuntu:22.04

# Install required build tools and dependencies
RUN apt-get update && apt-get install -y \
    g++ \
    make \
    libc6-dev \
    && rm -rf /var/lib/apt/lists/*

# Set working directory
WORKDIR /app

# Copy source files
COPY . .

# Create a Makefile for Linux
RUN echo '# Compiler configuration\n\
CXX = g++\n\
CXXFLAGS = -std=c++17 -Wall -Wextra -O2 -I./include\n\
LDFLAGS = -pthread -lrt\n\
\n\
# Directory structure\n\
SRC_DIR = src\n\
OBJ_DIR = obj\n\
BIN_DIR = bin\n\
\n\
# File lists\n\
SRCS = $(wildcard $(SRC_DIR)/*.cpp)\n\
OBJS = $(patsubst $(SRC_DIR)/%.cpp,$(OBJ_DIR)/%.o,$(SRCS))\n\
TARGET = $(BIN_DIR)/sensor_processor\n\
\n\
# Default target\n\
all: directories $(TARGET)\n\
\n\
directories:\n\
	mkdir -p $(OBJ_DIR) $(BIN_DIR)\n\
\n\
$(TARGET): $(OBJS)\n\
	$(CXX) $(OBJS) $(LDFLAGS) -o $@\n\
\n\
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp | directories\n\
	$(CXX) $(CXXFLAGS) -c $< -o $@\n\
\n\
clean:\n\
	rm -rf $(OBJ_DIR) $(BIN_DIR)\n\
\n\
.PHONY: all clean directories' > Makefile.linux

# Build using Linux Makefile
RUN make -f Makefile.linux clean && make -f Makefile.linux

# Run the sensor processor
CMD ["./bin/sensor_processor"] 