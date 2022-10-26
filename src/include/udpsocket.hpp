#pragma once

#include <tuple>
#include <variant>
#include <functional>
#include <string.h>
#include <unistd.h>
#include <netdb.h>
#include <sys/socket.h>
#include <sys/types.h>

struct Message
{
    bool ack;
    unsigned long msg_id;
    unsigned long src_id;
    unsigned long dst_id;
};

class UdpSocket
{
public:
    /**
     * @brief Construct a new Udp Socket object and binds it to the host
     *
     * @param host_ip host ip address
     * @param host_port host port number
     */
    UdpSocket(in_addr_t host_ip, in_port_t host_port);

    /**
     * @brief Destroy the Udp Socket object, closes the underlying file descriptor
     *
     */
    ~UdpSocket();

    /**
     * @brief Send a message through a UDP channel
     *
     * @param msg a pointer to the Message to be sent
     * @param dst_ip the destination ip address
     * @param dst_port the destination port number
     */
    ssize_t send(const Message *msg, in_addr_t dst_ip, in_port_t dst_port);

    /**
     * @brief Waits for a message to be received on the UDP channel
     *
     * @param msg a pointer to the Message to be received
     */
    std::tuple<in_addr_t, in_port_t> receive(Message *msg);

private:
    int sockfd;
};