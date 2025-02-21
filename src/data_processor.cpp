#include "data_processor.hpp"
#include <numeric>

namespace sensor {

// Constructor: Initialize processor with config and simulator reference, set up IPC
DataProcessor::DataProcessor(const Config& config, SensorSimulator& simulator)
    : m_config(config)
    , m_simulator(simulator)
    , m_running(false)
    , m_msg_counter(0)
{
    // Initialize IPC manager in sender mode
    if (m_ipc_manager.initialize(true) != ErrorCode::SUCCESS) {
        throw std::runtime_error("Failed to initialize IPC manager");
    }
}

// Destructor: Ensure processing thread is stopped
DataProcessor::~DataProcessor() {
    stop();
}

// Start: Begin data processing in a separate thread if not already running
void DataProcessor::start() {
    if (!m_running) {
        m_running = true;
        m_thread = std::thread(&DataProcessor::processingLoop, this);
    }
}

// Stop: Terminate processing thread and wait for it to finish
void DataProcessor::stop() {
    if (m_running) {
        m_running = false;
        // Join thread to ensure proper cleanup and prevent resource leaks
        if (m_thread.joinable()) {
            m_thread.join();
        }
    }
}

// ProcessingLoop: Main loop that processes sensor data and computes moving averages
void DataProcessor::processingLoop() {
    // Create buffer to store data window for moving average calculation
    CircularBuffer<SensorData> window_buffer(m_config.moving_avg_window);
    
    while (m_running) {
        // Attempt to get latest sensor reading
        if (auto data = m_simulator.getLatestData()) {
            // Add new data to window buffer
            window_buffer.push(*data);
            
            // Get window of data for moving average calculation
            auto window = window_buffer.getWindow(m_config.moving_avg_window);
            if (!window.empty()) {
                // Compute moving averages for all sensors
                auto avg_values = computeMovingAverage(window);
                
                // Create and send message with processed data
                MQMessage msg{
                    m_msg_counter++,
                    avg_values,
                    data->timestamp
                };
                
                m_ipc_manager.sendMessage(msg);
            }
        }
        
        // Sleep for half the sampling interval to ensure no data is missed
        std::this_thread::sleep_for(std::chrono::milliseconds(m_config.sampling_rate_ms / 2));
    }
}

// ComputeMovingAverage: Calculate moving average for each sensor from window of readings
std::array<double, NUM_SENSORS> DataProcessor::computeMovingAverage(
    const std::vector<SensorData>& window)
{
    std::array<double, NUM_SENSORS> averages{};
    
    // Return zero-initialized array if window is empty
    if (window.empty()) {
        return averages;
    }
    
    // Compute average for each sensor independently
    for (size_t sensor = 0; sensor < NUM_SENSORS; ++sensor) {
        double sum = 0.0;
        // Sum all readings for current sensor
        for (const auto& data : window) {
            sum += data.values[sensor];
        }
        // Calculate average by dividing sum by window size
        averages[sensor] = sum / window.size();
    }
    
    return averages;
}

} // namespace sensor 