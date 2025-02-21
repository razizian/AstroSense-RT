#pragma once

#include "common.hpp"
#include <fcntl.h>
#include <sys/stat.h>
#include <optional>

namespace sensor {

// IPCManager class: Manages inter-process communication using POSIX message queues
class IPCManager {
public:
    // Default constructor
    IPCManager();
    
    // Destructor ensures message queue is properly closed
    ~IPCManager();

    // Disable copy operations to prevent multiple queue handles
    IPCManager(const IPCManager&) = delete;
    IPCManager& operator=(const IPCManager&) = delete;

    // Move operations
    IPCManager(IPCManager&&) noexcept = default;
    IPCManager& operator=(IPCManager&&) noexcept = default;

    // Initialize the message queue in either sender or receiver mode
    ErrorCode initialize(bool is_sender);
    
    // Send a message to the queue
    ErrorCode sendMessage(const MQMessage& msg);
    
    // Receive a message from the queue
    std::optional<MQMessage> receiveMessage();
    
    // Clean up resources and close the message queue
    void cleanup();

private:
    mqd_t m_queue;           // Message queue descriptor
    bool m_is_initialized;    // Flag indicating if queue is initialized
    bool m_is_sender;        // Flag indicating if this instance is a sender
    
    // Message queue configuration constants
    static constexpr mode_t QUEUE_PERMISSIONS = 0660;  // rw-rw----
    static constexpr int MAX_MESSAGES = 10;           // Maximum messages in queue
};

} // namespace sensor 