#include "ipc_manager.hpp"
#include <errno.h>
#include <string.h>

#ifdef __APPLE__
// Mock implementations for macOS (IDE only) - real implementation uses POSIX message queues
struct mq_attr {
    long mq_flags;    // Message queue flags
    long mq_maxmsg;   // Maximum number of messages
    long mq_msgsize;  // Maximum message size
    long mq_curmsgs;  // Number of messages currently in queue
};

// Mock function implementations that return error codes
inline mqd_t mq_open(const char*, int, ...) { return -1; }
inline int mq_send(mqd_t, const char*, size_t, unsigned int) { return -1; }
inline ssize_t mq_receive(mqd_t, char*, size_t, unsigned int*) { return -1; }
inline int mq_close(mqd_t) { return 0; }
inline int mq_unlink(const char*) { return 0; }
#else
#include <mqueue.h>
#endif

namespace sensor {

// Constructor: Initialize member variables to safe defaults
IPCManager::IPCManager()
    : m_queue(MQ_INVALID)
    , m_is_initialized(false)
    , m_is_sender(false)
{}

// Destructor: Clean up message queue resources
IPCManager::~IPCManager() {
    cleanup();
}

// Initialize: Set up message queue for either sending or receiving
ErrorCode IPCManager::initialize(bool is_sender) {
    m_is_sender = is_sender;
    
    // Configure message queue attributes
    struct mq_attr attr;
    attr.mq_flags = 0;                    // Default flags
    attr.mq_maxmsg = MAX_MESSAGES;        // Maximum messages in queue
    attr.mq_msgsize = sizeof(MQMessage);  // Size of each message
    attr.mq_curmsgs = 0;                  // Current number of messages

    if (is_sender) {
        // Create queue with write-only access
        m_queue = mq_open(QUEUE_NAME, O_WRONLY | O_CREAT | O_NONBLOCK,
                         QUEUE_PERMISSIONS, &attr);
    } else {
        // Open queue with read-only access
        m_queue = mq_open(QUEUE_NAME, O_RDONLY | O_NONBLOCK, QUEUE_PERMISSIONS, nullptr);
    }

    // Check if queue was opened successfully
    if (m_queue == MQ_INVALID) {
        return ErrorCode::QUEUE_OPEN_ERROR;
    }

    m_is_initialized = true;
    return ErrorCode::SUCCESS;
}

// SendMessage: Send a message to the queue
ErrorCode IPCManager::sendMessage(const MQMessage& msg) {
    // Verify manager is initialized and in sender mode
    if (!m_is_initialized || !m_is_sender) {
        return ErrorCode::QUEUE_SEND_ERROR;
    }

    // Attempt to send message to queue
    if (mq_send(m_queue, reinterpret_cast<const char*>(&msg), 
                sizeof(MQMessage), 0) == -1) {
        if (errno == EAGAIN) {
            // Queue is full, non-blocking call would block
            return ErrorCode::BUFFER_FULL;
        }
        return ErrorCode::QUEUE_SEND_ERROR;
    }

    return ErrorCode::SUCCESS;
}

// ReceiveMessage: Attempt to receive a message from the queue
std::optional<MQMessage> IPCManager::receiveMessage() {
    // Verify manager is initialized and in receiver mode
    if (!m_is_initialized || m_is_sender) {
        return std::nullopt;
    }

    // Attempt to receive message from queue
    MQMessage msg;
    ssize_t bytes_read = mq_receive(m_queue, reinterpret_cast<char*>(&msg),
                                  sizeof(MQMessage), nullptr);

    if (bytes_read == -1) {
        if (errno == EAGAIN) {
            // No message available, non-blocking call would block
            return std::nullopt;
        }
        return std::nullopt;
    }

    return msg;
}

// Cleanup: Close and unlink message queue
void IPCManager::cleanup() {
    if (m_is_initialized) {
        // Close the queue handle
        mq_close(m_queue);
        if (m_is_sender) {
            // Only sender should unlink the queue
            mq_unlink(QUEUE_NAME);
        }
        m_is_initialized = false;
    }
}

} // namespace sensor 