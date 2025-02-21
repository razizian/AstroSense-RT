#pragma once

namespace sensor {

// Use .inl because it's a template implementation and we need it for compiler to find it
// Constructor: Initializes buffer with specified size and empty state
template<typename T>
CircularBuffer<T>::CircularBuffer(size_t size)
    : m_size(size)
    , m_buffer(size)
    , m_head(0)
    , m_tail(0)
    , m_full(false)
{}

// Push: Adds new item to the buffer, overwrites oldest item if full
template<typename T>
bool CircularBuffer<T>::push(const T& item) {
    std::lock_guard<std::mutex> lock(m_mutex);
    
    // Store item at head position
    m_buffer[m_head] = item;
    m_head = (m_head + 1) % m_size;
    
    if (m_full) {
        // Buffer is full, move tail to overwrite oldest item
        m_tail = (m_tail + 1) % m_size;
    } else {
        // Update full flag if head catches up to tail
        m_full = (m_head == m_tail);
    }
    
    return true;
}

// Pop: Removes and returns oldest item from buffer
template<typename T>
std::optional<T> CircularBuffer<T>::pop() {
    std::lock_guard<std::mutex> lock(m_mutex);
    
    // Return empty optional if buffer is empty
    if (empty()) {
        return std::nullopt;
    }

    // Get item at tail and advance tail position
    T item = m_buffer[m_tail];
    m_tail = (m_tail + 1) % m_size;
    m_full = false;
    
    return item;
}

// GetWindow: Returns vector of most recent items up to window_size
template<typename T>
std::vector<T> CircularBuffer<T>::getWindow(size_t window_size) const {
    std::lock_guard<std::mutex> lock(m_mutex);
    std::vector<T> window;
    
    // Return empty vector if buffer is empty or window size is 0
    if (empty() || window_size == 0) {
        return window;
    }

    // Calculate actual window size (minimum of requested size and available items)
    size_t count = std::min(window_size, size());
    window.reserve(count);

    // Calculate starting position and copy items to window
    size_t current = (m_tail + size() - count) % m_size;
    for (size_t i = 0; i < count; ++i) {
        window.push_back(m_buffer[current]);
        current = (current + 1) % m_size;
    }

    return window;
}

// Empty: Returns true if buffer contains no items
template<typename T>
bool CircularBuffer<T>::empty() const {
    return !m_full && (m_head == m_tail);
}

// Full: Returns true if buffer cannot accept more items
template<typename T>
bool CircularBuffer<T>::full() const {
    return m_full;
}

// Size: Returns current number of items in buffer
template<typename T>
size_t CircularBuffer<T>::size() const {
    if (m_full) {
        return m_size;
    }
    return (m_head - m_tail + m_size) % m_size;
}

// Capacity: Returns maximum number of items buffer can hold
template<typename T>
size_t CircularBuffer<T>::capacity() const {
    return m_size;
}

} // namespace sensor 