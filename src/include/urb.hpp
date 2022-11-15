#pragma once

#include <set>
#include "beb.hpp"

namespace da
{
    class uniform_reliable_broadcast : public best_effort_broadcast
    {
    public:
        uniform_reliable_broadcast(address, udp_socket, std::vector<address> &);
        ~uniform_reliable_broadcast();
        void broadcast(std::string &msg) override;
        void on_receive(std::function<void(std::string &, address &)>) override;
        inline bool can_deliver(address, message_id);

    protected:
        address self;
        message_id next_id;
        std::mutex pending_mutex;
        std::map<address, std::set<message_id>> pending;
        std::map<address, std::set<message_id>> delivered;
        std::map<address, std::map<message_id, size_t>> acks;

    private:
        typedef best_effort_broadcast beb;
    };
}