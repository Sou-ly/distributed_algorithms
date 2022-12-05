#pragma once

#include <netdb.h>
#include <string>
#include <optional>

namespace da
{
    typedef unsigned long message_id;

    /**
     * @brief Serializes a a given object of type T into a std::string of size sizeof(T)
     *
     * @tparam T the type of the object
     * @param t the object to be serialized
     * @return std::string the serialized object
     */
    template <typename T>
    std::string pack(T &t)
    {
        size_t size = sizeof(T);
        std::string dgram;
        dgram.reserve(size);
        for (size_t i = 1; i <= size; i++)
        {
            dgram.push_back(static_cast<char>(t >> (8 * (size - i)) & 0xFF));
        }
        return dgram;
    }

    /**
     * @brief Deserializes the first sizeof(T) leading bytes of a given datagram into an object of type T
     * 
     * @tparam T the type of the object to be created
     * @param dgram the datagram to partially deserialize
     * @return T the deserialized object
     */
    template <typename T>
    T unpack(std::string &dgram)
    {
        size_t size = sizeof(T);
        T t = 0;
        for (size_t i = 0; i < size; i++)
        {
            t |= (dgram[i] & 0xFF) << (8 * (size - i - 1));
        }
        dgram.erase(0, size);
        return t;
    }
}