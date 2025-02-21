// Prevent multiple inclusion of this header file
#pragma once

// Include required header files
#include "common.hpp"
#include <mutex>
#include <optional>
#include <vector>

namespace sensor {

// Thread-safe circular buffer implementation for generic type T
template<typename T>
class CircularBuffer {
public:
    // Constructor that initializes the buffer with specified size
    explicit CircularBuffer(size_t size);

    // Big 5
    CircularBuffer(const CircularBuffer&) = delete;
    CircularBuffer& operator=(const CircularBuffer&) = delete;
    CircularBuffer(CircularBuffer&&) noexcept = default;
    CircularBuffer& operator=(CircularBuffer&&) noexcept = default;
    ~CircularBuffer() = default;

    // Add an item to the buffer, returns false if buffer is full
    bool push(const T& item);
    
    // Remove and return the oldest item from the buffer
    std::optional<T> pop();
    
    // Get a window of most recent items for processing
    std::vector<T> getWindow(size_t window_size) const;

    // Buffer state query functions
    bool empty() const;      // Check if buffer is empty
    bool full() const;       // Check if buffer is full
    size_t size() const;     // Get current number of items in buffer
    size_t capacity() const; // Get maximum capacity of buffer

private:
    const size_t m_size;     // Fixed capacity of the buffer
    std::vector<T> m_buffer; // Underlying storage for buffer elements
    size_t m_head;          // Index for next write position
    size_t m_tail;          // Index for next read position
    bool m_full;            // Flag indicating buffer is full
    mutable std::mutex m_mutex; // Mutex for thread-safe operations
};

} // namespace sensor

// Include template implementation
#include "circular_buffer.inl" 