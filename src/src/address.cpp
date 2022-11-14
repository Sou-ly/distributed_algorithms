#include "address.hpp"

namespace da
{
    template <>
    in_port_t unpack<in_port_t>(std::string &dgram)
    {
        int p = dgram[0] << 8 | dgram[1];
        in_port_t port = static_cast<in_port_t>(p & 0xFFFF);
        dgram.erase(0, 4);
        return port;
    }

    template <>
    std::string pack<address>(address &addr)
    {
        std::string dgram;
        dgram += pack<in_addr_t>(addr.ip);
        dgram += pack<in_port_t>(addr.port);
        return dgram;
    }

    template <>
    address unpack<address>(std::string &dgram)
    {
        return address{unpack<in_addr_t>(dgram), unpack<in_port_t>(dgram)};
    }
}