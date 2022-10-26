#include "udpsocket.hpp"

UdpSocket::UdpSocket(in_addr_t host_ip, in_port_t host_port)
{
    // create socket
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);

    // make it reusable when closed
    int optval = 1;
    setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof optval);

    sockaddr_in saddr;
    memset(&saddr, 0, sizeof(sockaddr_in));
    saddr.sin_addr.s_addr = host_ip;
    saddr.sin_port = host_port;
    saddr.sin_family = AF_INET;

    // bind socket to host
    bind(sockfd, reinterpret_cast<const sockaddr *>(&saddr), sizeof(sockaddr_in));
}

UdpSocket::~UdpSocket()
{
    close(this->sockfd);
}

ssize_t UdpSocket::send(const Message *msg, in_addr_t dst_ip, in_port_t dst_port)
{
    sockaddr_in dest;
    memset(&dest, 0, sizeof(sockaddr_in));
    dest.sin_family = AF_INET;
    dest.sin_addr.s_addr = dst_ip;
    dest.sin_port = dst_port;
    ssize_t size = sendto(sockfd, reinterpret_cast<const void *>(msg), sizeof(Message), 0,
                          reinterpret_cast<const sockaddr *>(&dest), sizeof(sockaddr_in));

    if (size < 0)
    {
        perror("send");
    }

    return size;
}

std::tuple<in_addr_t, in_port_t> UdpSocket::receive(Message *msg)
{
    sockaddr_in src_addr;
    socklen_t socklen;
    if (recvfrom(sockfd, reinterpret_cast<void *>(msg), sizeof(Message), 0, reinterpret_cast<sockaddr *>(&src_addr), &socklen) < 0)
    {
        perror("receive");
    }
    return std::make_tuple(src_addr.sin_addr.s_addr, src_addr.sin_port);
}