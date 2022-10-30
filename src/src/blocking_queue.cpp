#include "blocking_queue.hpp"

namespace da
{
    template<typename T> blocking_queue<T>::blocking_queue() {}

    template<typename T> blocking_queue<T>::~blocking_queue() {}

    template<typename T>
    void blocking_queue<T>::push(const T& elem)
    {
        std::lock_guard<std::mutex> lock(this->mutex);
        this->queue.push(elem);
    }

    template <typename T>
    std::optional<T&> blocking_queue<T>::pop()
    {
        std::lock_guard<std::mutex> lock(this->mutex);
        if (this->queue.empty())
        {
            return std::nullopt;
        }
        T elem = std::move(this->queue.front());
        this->queue.pop();
        return elem;
    }

    template<typename T>
    bool blocking_queue<T>::is_empty()
    {
        std::lock_guard<std::mutex> lock(this->mutex);
        return this->queue.empty();
    }
}