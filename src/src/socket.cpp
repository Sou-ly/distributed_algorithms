#include "socket.hpp"

namespace da
{

    socket_descriptor::socket_descriptor(int fd) : sockfd(fd) {}

    udp_socket socket_descriptor::bind(address &host)
    {
        // create udp socket
        int fd = socket(AF_INET, SOCK_DGRAM, 0);

        // make it reusable when closed
        int optval = 1;
        setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof optval);

        sockaddr_in saddr;
        memset(&saddr, 0, sizeof(sockaddr_in));
        saddr.sin_addr.s_addr = host.ip;
        saddr.sin_port = host.port;
        saddr.sin_family = AF_INET;

        // associated address of the host process to the socket
        ::bind(fd, reinterpret_cast<const sockaddr *>(&saddr), sizeof(sockaddr_in));

        // put the descriptor on the heap and make it shared
        return udp_socket(new socket_descriptor(fd));
    }

    socket_descriptor::~socket_descriptor()
    {
        close(this->sockfd);
    }

    ssize_t socket_descriptor::write(const void *buf, size_t len, const address &dest)
    {
        sockaddr_in addr;
        addr.sin_addr.s_addr = dest.ip;
        addr.sin_port = dest.port;
        addr.sin_family = AF_INET;
        ssize_t retsize = sendto(sockfd, buf, len, 0, reinterpret_cast<sockaddr *>(&addr), sizeof(sockaddr_in));
        return retsize;
    }

    size_t socket_descriptor::read(void *buf, size_t max_len, address &src)
    {
        sockaddr_in addr;
        socklen_t socklen;
        ssize_t retsize = recvfrom(sockfd, buf, max_len, 0,
                                   reinterpret_cast<sockaddr *>(&addr), &socklen);
        src = {addr.sin_addr.s_addr, addr.sin_port};
        if (retsize < 0)
        {
            return 0;
        }
        else
        {
            return retsize;
        }
    }

}