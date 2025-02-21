#pragma once

#include "common.hpp"
#include "circular_buffer.hpp"
#include <atomic>
#include <random>
#include <thread>

namespace sensor {

// SensorSimulator class: Simulates multiple sensors generating data in real-time
class SensorSimulator {
public:
    // Constructor that initializes the simulator with configuration parameters
    explicit SensorSimulator(const Config& config);
    
    // Destructor ensures proper cleanup of resources
    ~SensorSimulator();

    // Disable copy operations to prevent multiple instances sharing resources
    SensorSimulator(const SensorSimulator&) = delete;
    SensorSimulator& operator=(const SensorSimulator&) = delete;

    // Move operations
    SensorSimulator(SensorSimulator&&) noexcept = delete;
    SensorSimulator& operator=(SensorSimulator&&) noexcept = delete;

    // Start the sensor simulation in a separate thread
    void start();
    
    // Stop the sensor simulation and cleanup resources
    void stop();
    
    // Retrieve the most recent sensor data, returns empty optional if no data available
    std::optional<SensorData> getLatestData();

private:
    // Main simulation loop that runs in a separate thread
    void simulationLoop();
    
    // Generate simulated sensor values using normal distribution
    std::array<double, NUM_SENSORS> generateSensorValues();

    // Configuration parameters for the simulator
    Config m_config;
    
    // Circular buffer to store sensor data
    CircularBuffer<SensorData> m_buffer;
    
    // Background thread for simulation
    std::thread m_thread;
    
    // Atomic flag for thread synchronization
    std::atomic<bool> m_running;
    
    // Random number generation components
    std::mt19937 m_rng;
    // Normal distribution models real-world sensor noise patterns the closests
    std::array<std::normal_distribution<double>, NUM_SENSORS> m_distributions;
};

} // namespace sensor 