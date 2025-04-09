# Spacecraft Real-Time Sensor Data Processing System

A real-time sensor data processing module for an embedded Linux spacecraft system. The system simulates sensor data, processes it using a moving average, and communicates between processes using POSIX Message Queues.

## System Overview

The system implements a multi-threaded sensor data processing pipeline with three main components:

1. **Sensor Simulator (`SensorSimulator` class)**
   - Simulates 6 different sensor types (temperature, pressure, humidity, etc.)
   - Uses normal distribution for realistic sensor value generation
   - Thread-safe circular buffer for data storage
   - Configurable sampling rate

2. **Data Processor (`DataProcessor` class)**
   - Computes moving averages over configurable time windows
   - Thread-safe implementation with IPC message sending
   - Efficient data processing with circular buffer window management
   - Non-blocking operation for real-time performance

3. **Output Handler (`OutputHandler` class)**
   - Formatted display of processed sensor data
   - Real-time timestamp conversion and display
   - Clean shutdown handling
   - Non-blocking message reception

## Core Components

### Circular Buffer (`CircularBuffer` class)
- Generic, thread-safe circular buffer implementation
- Supports window-based data access for moving averages
- Efficient memory usage with fixed-size allocation
- RAII-compliant resource management

### IPC Manager (`IPCManager` class)
- POSIX Message Queue wrapper for inter-process communication
- Non-blocking operations for real-time performance
- Automatic resource cleanup
- Error handling with detailed status codes

### Common Definitions (`common.hpp`)
- System-wide constants and configurations
- Sensor metadata and simulation parameters
- Data structures for sensor readings and messages
- Error code definitions

## IPC: POSIX Message Queues

The system uses POSIX Message Queues for several key advantages:

1. **Real-Time Performance**
   - Non-blocking operations via O_NONBLOCK flag
   - Priority-based message delivery
   - Predictable behavior for real-time systems
   - Automatic kernel-level synchronization

2. **Message-Oriented Design**
   - Built-in message framing
   - Type-safe message passing using `MQMessage` struct
   - Fixed message size for predictable performance
   - Automatic buffer management

3. **Reliability Features**
   - Kernel-managed queue lifecycle
   - Automatic buffer management
   - Built-in flow control
   - Error reporting via errno

4. **Advantages over Alternatives**
   - Pipes: MQs support multiple readers/writers and message priorities
   - Shared Memory: MQs handle synchronization automatically
   - Unix Domain Sockets: MQs are lighter weight for simple message passing
   - System V MQs: POSIX MQs have a cleaner API and better real-time support

## Data Flow Walkthrough

1. **Program Initialization** (`main.cpp`)
   - System configures sampling rate and moving average window
   - Initializes SensorSimulator, DataProcessor, and OutputHandler
   - Sets up signal handling for graceful shutdown

2. **Data Generation** (`sensor_simulator.cpp`)
   - SensorSimulator runs in dedicated thread
   - Generates synthetic sensor data using normal distribution
   - Stores readings in thread-safe CircularBuffer
   - Sampling occurs at configured rate (default 10Hz)

3. **Data Processing** (`data_processor.cpp`)
   - DataProcessor polls SensorSimulator's buffer
   - Computes moving averages over configured window
   - Packages processed data into MQMessage structure
   - Sends via POSIX Message Queue using IPCManager

4. **Data Output** (`output_handler.cpp`)
   - OutputHandler receives MQMessages from queue
   - Formats data with timestamps and units
   - Displays real-time sensor readings to console

And add this diagram (using ASCII for simplicity, but could be replaced with a proper image):

```
+----------------+     +-----------------+     +----------------+
| SensorSimulator|     | DataProcessor  |     | OutputHandler  |
|                |     |                |     |                |
| [CircBuffer]   |---->| [MovingAvg]    |---->| [Console Out] |
|                |     |                |     |                |
| Thread 1       |     | Thread 2       |     | Thread 3      |
+----------------+     +-----------------+     +----------------+
                         ^   |
                         |   v
                    POSIX Message Queue
```

This addition would help developers better understand:
- The execution flow
- Thread interactions
- Data transformation stages
- IPC communication points

Sources:
- Main program flow: src/main.cpp
- Component interactions: src/data_processor.cpp, src/sensor_simulator.cpp, src/output_handler.cpp 

## Example Output
The system outputs formatted sensor readings with timestamps and moving averages:

```
Starting sensor data processing system...

[2025-02-21 21:34:56] Message ID: 0
Temperature     Avg:    25.31 °C
Pressure        Avg:   101.12 kPa
Humidity        Avg:    44.38 %
Acceleration    Avg:    -0.02 m/s²
Magnetic Field  Avg:    44.64 µT
Gyroscope       Avg:     0.68 °/s

[2025-02-21 21:34:56] Message ID: 1
Temperature     Avg:    28.31 °C
Pressure        Avg:   101.38 kPa
Humidity        Avg:    40.27 %
Acceleration    Avg:     0.19 m/s²
Magnetic Field  Avg:    43.95 µT
Gyroscope       Avg:     0.36 °/s
```

Each reading includes:
- Timestamp in ISO 8601 format
- Message ID for tracking
- Moving averages for all 6 sensors with units
- Updates at configured sampling rate (default: 10Hz) 

## Building and Running

### Prerequisites
- C++17 compiler (g++ recommended)
- POSIX Message Queues (libmqueue-dev)
- pthread library
- Make build system

### Native Build
```bash
# Install dependencies (Ubuntu/Debian)
sudo apt-get install g++ make libmqueue-dev

# Build
make clean && make

# Run
./bin/sensor_processor
```

### Docker Build
```bash
# Build container
docker build -t sensor-processor .

# Run container
docker run --rm sensor-processor
```

## Configuration

The system is configurable through the `Config` struct in `common.hpp`:

```cpp
struct Config {
    int sampling_rate_ms = 100;   // Sampling rate in milliseconds (default: 10Hz)
    int moving_avg_window = 10;   // Moving average window size (default: 1 second)
};
```

## Implementation Details

### Modern C++ Features
- Smart pointers for automatic resource management
- std::optional for safe data access
- Thread-safe containers and operations
- RAII principles throughout

### Thread Safety
- Mutex-protected circular buffer operations
- Atomic flags for thread synchronization
- Thread-safe message queue operations
- Safe shutdown handling

### Error Handling
- Exception-safe resource management
- Detailed error codes and status reporting
- Graceful cleanup on errors
- Signal handling for clean shutdown

### Performance Optimizations
- Non-blocking I/O operations
- Efficient circular buffer implementation
- Minimal memory allocation during operation
- Optimized moving average computation

## Development Notes

The codebase is thoroughly commented with detailed explanations of:
- Class and function purposes
- Algorithm implementations
- Thread safety considerations
- Error handling strategies
- Performance considerations
- Resource management approaches
