#pragma once

#include "perfectlink.hpp"

namespace da
{
    class best_effort_broadcast
    {
    public:
        best_effort_broadcast(udp_socket socket, std::vector<address> &);
        ~best_effort_broadcast();
        virtual void broadcast(std::string &msg);
        virtual void on_receive(std::function<void(std::string &, address &)>);

    protected:
        perfect_link pp2p;
        std::vector<std::function<void(std::string &, address &)>> handlers;
        std::vector<address> peers;
    };
}