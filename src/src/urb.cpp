#include "urb.hpp"

namespace da
{
    uniform_reliable_broadcast::uniform_reliable_broadcast(address host, udp_socket socket, std::vector<address> &peers)
        : best_effort_broadcast(socket, peers)
    {
        lsn = 0;
        self = host;
        pending = {};
        delivered = {};
        acks = {};
    }

    uniform_reliable_broadcast::~uniform_reliable_broadcast() {}

    void uniform_reliable_broadcast::broadcast(std::string &msg)
    {
        lsn++;
        std::string datagram;
        datagram.reserve(sizeof(address) + sizeof(message_id) + msg.size());
        // construct header
        datagram += pack<address>(self);   // origin
        datagram += pack<message_id>(lsn); // msg id
        // add payload
        datagram.append(msg);
        std::lock_guard<std::mutex> lock(pending_mutex);
        pending[self].insert(lsn);
        beb::broadcast(datagram);
    }

    void uniform_reliable_broadcast::on_receive(std::function<void(std::string &, address &)> urb_deliver)
    {
        beb::on_receive([&](std::string &m, address &sender) -> void
                        {
                            std::string copy(m);
                            address origin = unpack<address>(m);
                            message_id id = unpack<message_id>(m);
                            acks[origin][id].insert(sender);
                            std::lock_guard<std::mutex> lock(pending_mutex);
                            if (pending[origin].find(id) == pending[origin].end())
                            {
                                pending[origin].insert(id);
                                beb::broadcast(copy);
                            }
                            // at this point, the message is always in pending
                            if (can_deliver(origin, id) && delivered[origin].find(id) == delivered[origin].end())
                            {
                                delivered[origin].insert(id);
                                urb_deliver(m, origin);
                            } });
    }

    inline bool uniform_reliable_broadcast::can_deliver(address origin, message_id id)
    {
        return acks[origin][id].size() > peers.size() / 2;
    }
}