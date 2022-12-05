#pragma once

#include "perfectlink.hpp"

namespace da
{
    class best_effort_broadcast
    {
    public:
        /**
         * @brief Construct a new best effort broadcast object
         * 
         * @param socket a shared pointer to a socket_descriptor
         * @param peers a reference to the list of addresses of the peers (including host)
         */
        best_effort_broadcast(udp_socket socket, std::vector<address> & peers);

        /**
         * @brief Destroy the best effort broadcast object
         * 
         */
        ~best_effort_broadcast();

        /**
         * @brief Broadcast a message to all peers involved in the broadcast object
         * 
         * @param msg a reference to the message to broadcast
         */
        virtual void broadcast(std::string &msg);

        /**
         * @brief Add a callback for receive events, starts listening to receive events on first call
         * 
         * @param beb_deliver the callback to be called on a receive event
         */
        virtual void on_receive(std::function<void(std::string &, address &)> beb_deliver);

    protected:
        perfect_link pp2p;
        std::vector<std::function<void(std::string &, address &)>> handlers;
        std::vector<address> peers;
    };
}