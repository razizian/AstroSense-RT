#pragma once

#include <array>
#include <chrono>
#include <cstdint>
#include <string>
#include <string_view>

#ifdef __APPLE__
// Mock mqueue types for macOS (IDE only)
typedef int mqd_t;
#define MQ_INVALID (-1)
#else
#include <mqueue.h>
#define MQ_INVALID ((mqd_t)-1)
#endif

namespace sensor {

// System-wide constants
constexpr size_t NUM_SENSORS = 6;           // Total number of sensors in the system
constexpr size_t BUFFER_SIZE = 100;         // Size of circular buffer for sensor data
constexpr size_t MAX_MSG_SIZE = 256;        // Maximum size of IPC message queue messages
constexpr const char* QUEUE_NAME = "/sensor_mq";  // Name of the IPC message queue

// Structure defining metadata for each sensor type
struct SensorMetadata {
    std::string_view name;   // Display name of the sensor
    std::string_view unit;   // Unit of measurement
    double mean;             // Expected mean value for simulation
    double stddev;          // Standard deviation for value simulation
};

// Array of sensor definitions with their simulation parameters
constexpr std::array<SensorMetadata, NUM_SENSORS> SENSORS = {{
    {"Temperature",     "°C",   25.0,  2.0},  // Room temperature with ±2°C variation
    {"Pressure",        "kPa", 101.3,  0.5},  // Atmospheric pressure with small variations
    {"Humidity",        "%",    45.0,  5.0},  // Relative humidity with moderate variations
    {"Acceleration",    "m/s²",  0.0,  0.5},  // Acceleration near rest with small noise
    {"Magnetic Field",  "µT",    45.0, 2.0},  // Earth's magnetic field with variations
    {"Gyroscope",      "°/s",    0.0, 1.0}   // Angular velocity near rest with noise
}};

// Structure for raw sensor data readings
struct SensorData {
    std::array<double, NUM_SENSORS> values;     // Array of sensor readings
    std::chrono::system_clock::time_point timestamp;  // Timestamp of the readings
};

// Structure for processed sensor data messages
struct MQMessage {
    uint64_t msg_id;                            // Unique message identifier
    std::array<double, NUM_SENSORS> avg_values; // Moving average of sensor values
    std::chrono::system_clock::time_point timestamp;  // Timestamp of the processed data
};

// Configuration parameters for the sensor system
struct Config {
    int sampling_rate_ms = 100;   // Sampling rate in milliseconds (default: 100ms)
    int moving_avg_window = 10;   // Number of samples in moving average window (default: 10)
};

// Error codes for system operations
enum class ErrorCode {
    SUCCESS = 0,           // Operation completed successfully
    QUEUE_OPEN_ERROR,      // Failed to open message queue
    QUEUE_SEND_ERROR,      // Failed to send message to queue
    QUEUE_RECEIVE_ERROR,   // Failed to receive message from queue
    BUFFER_FULL,          // Circular buffer is full
    BUFFER_EMPTY          // Circular buffer is empty
};

} // namespace sensor 