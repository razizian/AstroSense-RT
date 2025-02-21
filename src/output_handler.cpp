#include "output_handler.hpp"
#include <iomanip>
#include <iostream>

namespace sensor {

// Constructor: Initialize output handler with config and set up IPC
OutputHandler::OutputHandler(const Config& config)
    : m_config(config)
    , m_running(false)
{
    // Initialize IPC manager in receiver mode
    if (m_ipc_manager.initialize(false) != ErrorCode::SUCCESS) {
        throw std::runtime_error("Failed to initialize IPC manager");
    }
}

// Destructor: Ensure output thread is stopped
OutputHandler::~OutputHandler() {
    stop();
}

// Start: Begin output handling in a separate thread if not already running
void OutputHandler::start() {
    if (!m_running) {
        m_running = true;
        m_thread = std::thread(&OutputHandler::outputLoop, this);
    }
}

// Stop: Terminate output thread and wait for it to finish
void OutputHandler::stop() {
    if (m_running) {
        m_running = false;
        // Join thread to ensure proper cleanup and prevent resource leaks
        if (m_thread.joinable()) {
            m_thread.join();
        }
    }
}

// OutputLoop: Main loop that receives and displays processed sensor data
void OutputHandler::outputLoop() {
    while (m_running) {
        // Check for new messages from the processor
        if (auto msg = m_ipc_manager.receiveMessage()) {
            printSensorData(*msg);
        }
        
        // Sleep for half the sampling interval to ensure responsive output
        std::this_thread::sleep_for(std::chrono::milliseconds(m_config.sampling_rate_ms / 2));
    }
}

// PrintSensorData: Format and display processed sensor data with timestamp
void OutputHandler::printSensorData(const MQMessage& msg) {
    // Convert timestamp to local time and format it
    auto time = std::chrono::system_clock::to_time_t(msg.timestamp);
    std::cout << "\n[" << std::put_time(std::localtime(&time), "%F %T") 
              << "] Message ID: " << msg.msg_id << "\n";
    
    // Set up floating-point output format
    std::cout << std::fixed << std::setprecision(2);

    // Display average values for each sensor with proper formatting
    for (size_t i = 0; i < NUM_SENSORS; ++i) {
        std::cout << std::left << std::setw(16) << SENSORS[i].name 
                  << "Avg: " << std::right << std::setw(8) << msg.avg_values[i] 
                  << " " << SENSORS[i].unit << "\n";
    }
    std::cout << std::endl;
}

} // namespace sensor 