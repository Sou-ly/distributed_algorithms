#pragma once

#include <cstring>
#include <memory>
#include <functional>
#include <sstream>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include "address.hpp"

namespace da
{
    class socket_descriptor;

    typedef std::shared_ptr<socket_descriptor> udp_socket;

    class socket_descriptor
    {
    public:
        /**
         * @brief Construct a new udp Socket object, binds it to the host and connects it to the remote process
         *
         * @param host the host address
         * @param host_port remote process address
         */
        static udp_socket bind(address& host);

        /**
         * @brief Destroy the socket_descriptor object, closes the underlying file descriptor
         *
         */
        ~socket_descriptor();

        /**
         * @brief Send a message through a UDP channel, non blocking.
         *
         * @param buf the message to send
         */
        ssize_t write(const void * buf, size_t len, const address &dest);

        /**
         * @brief Waits for a message to be received on the UDP channel, blocking call.
         *
         * @param buf a buffer for message to be received
         */
        ssize_t read(void * buf, size_t max_len, address &src);

    private:
        socket_descriptor(int sockfd);
        int sockfd;
    };
}