#pragma once

#include "perfectlink.hpp"

namespace da
{
    class best_effort_broadcast
    {
        public:
        best_effort_broadcast(udp_socket socket, std::vector<udp_sockaddr>&);
        ~best_effort_broadcast();
        void broadcast(std::string& msg);
        void on_receive(std::function<void(std::string&, udp_sockaddr&)>);
        protected:
        perfect_link pp2p;
        std::vector<std::function<void(std::string&, udp_sockaddr&)>> handlers;
        std::vector<udp_sockaddr> peers;
    };
}