#pragma once

#include <netdb.h>
#include <string>
#include <optional>

namespace da
{
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