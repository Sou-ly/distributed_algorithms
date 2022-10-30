#pragma once

#include <cstring>
#include <memory>
#include <functional>
#include <sstream>
#include <string.h>
#include <unistd.h>
#include <netdb.h>
#include <sys/socket.h>
#include <sys/types.h>

namespace da
{

    struct udp_sockaddr
    {
        in_addr_t ip;
        in_port_t port;

        bool operator==(const udp_sockaddr &sa) const {
            return ip == sa.ip && port == sa.port;
        }

        bool operator<(const udp_sockaddr &sa) const {
            return ip < sa.ip || port < sa.port;
        }
    };

    class socket_descriptor;

    typedef std::shared_ptr<socket_descriptor> udp_socket;

    class socket_descriptor
    {
    public:
        /**
         * @brief Construct a new udp Socket object, binds it to the host and connects it to the remote process
         *
         * @param host the host udp_sockaddr
         * @param host_port remote process udp_sockaddr
         */
        static udp_socket bind(udp_sockaddr& host);

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
        ssize_t write(const std::string &buf, const udp_sockaddr &dest);

        /**
         * @brief Waits for a message to be received on the UDP channel, blocking call.
         *
         * @param buf a buffer for message to be received
         */
        ssize_t read(std::string &buf, size_t max_len, udp_sockaddr &src);

    private:
        socket_descriptor(int sockfd);
        int sockfd;
    };
}