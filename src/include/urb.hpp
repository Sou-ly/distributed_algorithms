#pragma once

#include <set>
#include "beb.hpp"

namespace da
{
    class uniform_reliable_broadcast : public best_effort_broadcast
    {
    public:
        /**
         * @brief Construct a new uniform reliable broadcast object.
         *
         * @param host the current socket host
         * @param socket a shared pointer to a socket_descriptor
         * @param peers a reference to the list of addresses of the peers (including host)
         */
        uniform_reliable_broadcast(address host, udp_socket socket, std::vector<address> &peers);

        /**
         * @brief Destroy the uniform reliable broadcast object
         *
         */
        ~uniform_reliable_broadcast();

        /**
         * @brief Broadcast a message to all peers involved in the broadcast
         *
         * @param msg a reference to the message to broadcast
         */
        void broadcast(std::string &msg) override;

        /**
         * @brief Add a callback for receive events, starts listening to receive events on first call
         *
         * @param urb_deliver the callback to be called on a receive event
         */
        void on_receive(std::function<void(std::string &, address &)> urb_deliver) override;

    protected:
        inline bool can_deliver(address, message_id);
        std::vector<std::function<void(std::string &, address &)>> callbacks;
        address self;
        message_id sn;
        std::unordered_map<address, std::mutex> mutex;
        std::unordered_map<address, std::set<message_id>> pending;
        std::unordered_map<address, std::set<message_id>> delivered;
        std::unordered_map<address, std::unordered_map<message_id, std::set<address>>> acks;

    private:
        typedef best_effort_broadcast beb;
    };
}