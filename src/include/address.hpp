#pragma once

#include <netdb.h>
#include "util.hpp"

namespace da
{
    /**
     * @brief Represents the address of a process as a (in_addr_t, in_port_t) pair
     *
     */
    struct address
    {
        in_addr_t ip;   // the IP address of the process
        in_port_t port; // the port number of the process

        bool operator==(const address &sa) const
        {
            return ip == sa.ip && port == sa.port;
        }

        bool operator<(const address &sa) const
        {
            return ip < sa.ip || port < sa.port;
        }
    };

    /**
     * @brief A helper method to specialize std::hash<address>
     */
    template <class T>
    inline void hash_combine(std::size_t &seed, const T &v)
    {
        std::hash<T> hasher;
        seed ^= hasher(v) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
    }

    template <>
    in_port_t unpack<in_port_t>(std::string &dgram);

    template <>
    std::string pack<address>(address &addr);

    template <>
    address unpack<address>(std::string &dgram);
}

namespace std
{
    template <>
    struct hash<da::address>
    {
        inline size_t operator()(da::address const &adr) const
        {
            size_t seed = 0;
            da::hash_combine(seed, adr.ip);
            da::hash_combine(seed, adr.port);
            return seed;
        }
    };
}