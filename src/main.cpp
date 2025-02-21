// Header includes for core system components
#include "sensor_simulator.hpp"
#include "data_processor.hpp"
#include "output_handler.hpp"

// System header includes
#include <csignal>
#include <iostream>

// Using declaration for the sensor namespace
using namespace sensor;

namespace {
    // Global atomic flag for graceful shutdown
    std::atomic<bool> g_running{true};

    // Signal handler function for handling Ctrl+C (SIGINT)
    void signalHandler(int) {
        g_running = false;
    }
}

int main(int argc, char* argv[]) {
    try {
        // Register signal handler for graceful shutdown on Ctrl+C
        std::signal(SIGINT, signalHandler);
        
        // System configuration initialization
        Config config;
        config.sampling_rate_ms = 100;  // Set sampling rate to 10Hz (100ms intervals)
        config.moving_avg_window = 10;  // Configure 1-second moving average window (10 samples at 10Hz)
        
        // Initialize core system components
        SensorSimulator simulator(config);
        DataProcessor processor(config, simulator);
        OutputHandler output(config);
        
        // System startup notification
        std::cout << "Starting sensor data processing system...\n";

        // Start all system components in sequence
        simulator.start();
        processor.start();
        output.start();
        
        // Main program loop - runs until shutdown signal is received
        while (g_running) {
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }
        
        // Graceful shutdown sequence
        std::cout << "\nShutting down...\n";
        output.stop();
        processor.stop();
        simulator.stop();
        
        return 0;
    } catch (const std::exception& e) {
        // Error handling for any unhandled exceptions
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
} 