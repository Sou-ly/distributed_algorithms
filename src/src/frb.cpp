#include "frb.hpp"

namespace da
{
    fifo_reliable_broadcast::fifo_reliable_broadcast(address host, udp_socket socket, std::vector<address> &peers)
        : uniform_reliable_broadcast(host, socket, peers)
    {
        lsn = 0;
        for (auto const &peer : peers)
        {
            next[peer] = 1;
        }
    }

    fifo_reliable_broadcast::~fifo_reliable_broadcast() {}

    void fifo_reliable_broadcast::broadcast(std::string &msg)
    {
        lsn++;
        std::string dgram;
        dgram.reserve(sizeof(message_id) + msg.size());
        dgram += pack<message_id>(lsn);
        dgram.append(msg);
        urb::broadcast(dgram);
    }

    void fifo_reliable_broadcast::on_receive(std::function<void(std::string &, address &)> frb_deliver)
    {
        urb::on_receive([&](std::string &m, address &p) -> void
                        {
                            message_id sn = unpack<message_id>(m);
                            pending[p][sn] = m;
                            while (pending[p].count(next[p]))
                            {
                                frb_deliver(pending[p][next[p]], p);
                                pending[p].erase(next[p]);
                                next[p]++;
                            } });
    }
}