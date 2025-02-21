#include "sensor_simulator.hpp"
#include <chrono>

namespace sensor {

// Constructor: Initialize simulator with configuration and set up random number generators
SensorSimulator::SensorSimulator(const Config& config)
    : m_config(config)
    , m_buffer(BUFFER_SIZE)
    , m_running(false)
    , m_rng(std::random_device{}())
{
    // Initialize normal distributions for each sensor using metadata
    for (size_t i = 0; i < NUM_SENSORS; ++i) {
        m_distributions[i] = std::normal_distribution<double>(
            SENSORS[i].mean, SENSORS[i].stddev);
    }
}

// Destructor: Ensure simulation thread is stopped
SensorSimulator::~SensorSimulator() {
    stop();
}

// Start: Begin sensor simulation in a separate thread if not already running
void SensorSimulator::start() {
    if (!m_running) {
        m_running = true;
        // Create and start a new thread that runs the simulationLoop member function
        m_thread = std::thread(&SensorSimulator::simulationLoop, this);
    }
}

// Stop: Terminate simulation thread and wait for it to finish
void SensorSimulator::stop() {
    if (m_running) {
        m_running = false;
        // Join thread to ensure proper cleanup and prevent resource leaks
        if (m_thread.joinable()) {
            m_thread.join();
        }
    }
}

// GetLatestData: Retrieve and remove the most recent sensor reading
std::optional<SensorData> SensorSimulator::getLatestData() {
    return m_buffer.pop();
}

// SimulationLoop: Main loop that generates sensor data at specified intervals
void SensorSimulator::simulationLoop() {
    // More specific declarations are preferred over using namespace
    using std::chrono::system_clock;
    using std::chrono::milliseconds;
    
    while (m_running) {
        // Create new sensor data with current values and timestamp
        SensorData data{
            generateSensorValues(),
            system_clock::now()
        };
        
        // Store data in circular buffer
        m_buffer.push(data);
        
        // Wait for next sampling interval
        std::this_thread::sleep_for(milliseconds(m_config.sampling_rate_ms));
    }
}

// GenerateSensorValues: Create simulated readings for all sensors
std::array<double, NUM_SENSORS> SensorSimulator::generateSensorValues() {
    std::array<double, NUM_SENSORS> values;
    // Generate random values using normal distribution for each sensor
    for (size_t i = 0; i < NUM_SENSORS; ++i) {
        values[i] = m_distributions[i](m_rng);
    }
    return values;
}

} // namespace sensor 