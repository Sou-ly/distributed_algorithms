#pragma once

#include "urb.hpp"

namespace da
{
    class fifo_reliable_broadcast : public uniform_reliable_broadcast
    {
    public:
        /**
         * @brief Construct a new fifo reliable broadcast object built on top of uniform reliable broadcast
         *
         * @param host the current socket host
         * @param socket a shared pointer to a socket_descriptor
         * @param peers a reference to the list of addresses of the peers (including host)
         */
        fifo_reliable_broadcast(address host, udp_socket socket, std::vector<address> &peers);

        /**
         * @brief Destroy the fifo reliable broadcast object
         * 
         */
        ~fifo_reliable_broadcast();

        /**
         * @brief Broadcast a message to all peers involved in the broadcast object
         *
         * @param msg a reference to the message to broadcast
         */
        void broadcast(std::string &msg) override;

        /**
         * @brief Add a callback for receive events, starts listening to receive events on first call
         *
         * @param frb_deliver the callback to be called on a receive event
         */
        void on_receive(std::function<void(std::string &, address &)> frb_deliver) override;

    protected:
        message_id lsn;
        address self;
        std::unordered_map<address, message_id> next;
        std::unordered_map<address, std::unordered_map<message_id, std::string>> pending;

    private:
        typedef uniform_reliable_broadcast urb;
    };
}