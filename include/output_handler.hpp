// Prevent multiple inclusion of this header file
#pragma once

// Include required header files
#include "common.hpp"
#include "ipc_manager.hpp"
#include <atomic>
#include <thread>

namespace sensor {

// OutputHandler class: Handles the display and formatting of processed sensor data
class OutputHandler {
public:
    // Constructor that initializes the output handler with configuration parameters
    explicit OutputHandler(const Config& config);
    
    // Destructor ensures proper cleanup of resources
    ~OutputHandler();

    // Disable copy operations to prevent multiple instances sharing resources
    OutputHandler(const OutputHandler&) = delete;
    OutputHandler& operator=(const OutputHandler&) = delete;

    // Move operations
    OutputHandler(OutputHandler&&) noexcept = delete;
    OutputHandler& operator=(OutputHandler&&) noexcept = delete;

    // Start the output handling in a separate thread
    void start();
    
    // Stop the output handling and cleanup resources
    void stop();

private:
    // Main output loop that runs in a separate thread
    void outputLoop();
    
    // Format and print sensor data received through IPC
    void printSensorData(const MQMessage& msg);

    // Configuration parameters for the output handler
    Config m_config;
    
    // IPC manager for inter-process communication
    IPCManager m_ipc_manager;
    
    // Background thread for output handling
    std::thread m_thread;
    
    // Atomic flag for thread synchronization
    std::atomic<bool> m_running;
};

} // namespace sensor 