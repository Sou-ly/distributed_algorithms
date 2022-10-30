#pragma once

#include <map>
#include <thread>
#include <atomic>
#include <iostream>
#include "socket.hpp"
#include "blocking_queue.hpp"

namespace da
{
    void talk(std::atomic<bool>&, udp_socket, std::mutex&, std::vector<std::pair<udp_sockaddr, std::string>>&);
    void listen(std::atomic<bool>& listening,
                udp_socket socket,
                std::vector<std::string>& delivered,
                std::vector<std::function<void(std::string &, udp_sockaddr &)>>& callbacks);

    class perfect_link
    {
    public:
        /**
         * @brief Construct a new Perfect Link object
         *
         * @param socket
         */
        perfect_link(udp_socket socket);

        /**
         * @brief Destroy the Perfect Link object
         *
         */
        ~perfect_link();

        /**
         * @brief
         *
         * @param buf
         * @param dest
         * @return ssize_t
         */
        void send(const std::string &buf, const udp_sockaddr &dest);

        /**
         * @brief
         *
         * @param deliver
         */
        void upon_deliver(std::function<void(std::string &, udp_sockaddr &)> deliver);

    private:
        std::atomic<bool> listening, talking;
        unsigned long id_counter;
        std::vector<std::function<void(std::string &, udp_sockaddr &)>> handlers;
        std::vector<std::string> delivered;
        std::mutex mutex;
        std::vector<std::pair<udp_sockaddr, std::string>> sent;
        std::thread sender, listener;
        udp_socket socket;
    };
}