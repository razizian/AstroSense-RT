#pragma once

#include "common.hpp"
#include "sensor_simulator.hpp"
#include "ipc_manager.hpp"
#include <atomic>
#include <thread>

namespace sensor {

// DataProcessor class: Processes sensor data using moving average and manages IPC communication
class DataProcessor {
public:
    // Constructor that initializes the processor with config and sensor simulator reference
    explicit DataProcessor(const Config& config, SensorSimulator& simulator);
    
    // Destructor ensures proper cleanup of resources
    ~DataProcessor();

    // Disable copy operations to prevent multiple instances sharing resources
    DataProcessor(const DataProcessor&) = delete;
    DataProcessor& operator=(const DataProcessor&) = delete;

    // Move operations
    DataProcessor(DataProcessor&&) noexcept = delete;
    DataProcessor& operator=(DataProcessor&&) noexcept = delete;

    // Start the data processing in a separate thread
    void start();
    
    // Stop the data processing and cleanup resources
    void stop();

private:
    // Main processing loop that runs in a separate thread
    void processingLoop();
    
    // Compute moving average for each sensor using a window of historical data
    std::array<double, NUM_SENSORS> computeMovingAverage(const std::vector<SensorData>& window);

    // Configuration parameters for the processor
    Config m_config;
    
    // Reference to the sensor simulator for data acquisition
    SensorSimulator& m_simulator;
    
    // IPC manager for inter-process communication
    IPCManager m_ipc_manager;
    
    // Background thread for processing
    std::thread m_thread;
    
    // Atomic flag for thread synchronization
    std::atomic<bool> m_running;
    
    // Counter for tracking processed messages
    uint64_t m_msg_counter;
};

} // namespace sensor 