#pragma once

#include <netdb.h>
#include "util.hpp"

namespace da
{
    struct address
    {
        in_addr_t ip;
        in_port_t port;

        bool operator==(const address &sa) const
        {
            return ip == sa.ip && port == sa.port;
        }

        bool operator<(const address &sa) const
        {
            return ip < sa.ip || port < sa.port;
        }
    };

    template <>
    in_port_t unpack<in_port_t>(std::string &dgram);

    template <>
    std::string pack<address>(address &addr);

    template <>
    address unpack<address>(std::string &dgram);
}