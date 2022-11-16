#include "beb.hpp"

namespace da
{
    best_effort_broadcast::best_effort_broadcast(udp_socket socket, std::vector<address> &peers)
        : pp2p(socket, peers), peers(peers) {}

    best_effort_broadcast::~best_effort_broadcast() {}

    void best_effort_broadcast::broadcast(std::string &msg)
    {
        for (auto const &peer : peers)
        {
            pp2p.send(msg, peer);
        }
    }

    void best_effort_broadcast::on_receive(std::function<void(std::string &, address &)> handler)
    {
        handlers.push_back(handler);
        if (handlers.size() == 1)
        {
            pp2p.upon_deliver([&](std::string &msg, address &src) -> void
                {
                    for (auto const &handler : handlers)
                    {
                        handler(msg, src);
                } });
        }
    }
}