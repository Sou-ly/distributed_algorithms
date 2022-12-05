#pragma once

#include <map>
#include <thread>
#include <atomic>
#include <mutex>
#include <iostream>
#include <array>
#include "socket.hpp"

namespace da
{
    class perfect_link
    {
    public:
        /**
         * @brief Construct a new perfect link object
         *
         * @param socket a shared pointer to a socket_descriptor
         * @param peers a reference to a list of peers involved in the communications
         */
        perfect_link(udp_socket socket, std::vector<address> &peers);

        /**
         * @brief Destroy the perfect link object, stops listening to receive events on destruction.
         *
         */
        ~perfect_link();

        /**
         * @brief Send a message over a reliable udp channel to a single remote process.
         *
         * @param msg a reference to the message to be sent
         * @param dest the address of destination of the message
         */
        void send(const std::string &msg, const address &dest);

        /**
         * @brief Add a callback to be called on receive events, starts listening on first call.
         *
         * @param pp2p_deliver the callback to be called on a receive event
         */
        void upon_deliver(std::function<void(std::string &, address &)> pp2p_deliver);

    private:
        void talk();
        void listen();
        udp_socket socket;
        message_id sn;
        std::thread sender, listener;
        std::atomic<bool> listening, talking;
        std::vector<address> peers;
        std::vector<std::function<void(std::string &, address &)>> handlers;
        std::unordered_map<address, std::vector<message_id>> delivered;
        std::unordered_map<address, std::mutex> mutex;
        std::unordered_map<address, std::map<message_id, std::pair<void *, size_t>>> sent;
    };
}