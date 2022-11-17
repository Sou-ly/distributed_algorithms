#pragma once

#include "urb.hpp"

namespace da
{
    class fifo_reliable_broadcast : public uniform_reliable_broadcast
    {
    public:
        fifo_reliable_broadcast(address, udp_socket, std::vector<address> &);
        ~fifo_reliable_broadcast();
        void broadcast(std::string &msg) override;
        void on_receive(std::function<void(std::string &, address &)>) override;

    protected:
        message_id lsn;
        address self;
        std::map<address, message_id> next;
        std::map<address, std::map<message_id, std::string>> pending;

    private:
        typedef uniform_reliable_broadcast urb;
    };
}