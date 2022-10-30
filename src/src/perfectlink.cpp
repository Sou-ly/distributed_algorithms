#include "perfectlink.hpp"

namespace da
{
    perfect_link::perfect_link(udp_socket socket) : socket(socket)
    {
        talking = false;
        listening = false;
        id_counter = 0;
        delivered = {};
    }

    perfect_link::~perfect_link()
    {
        if (sender.joinable())
        {
            talking = false;
            sender.join();
        }
        if (listener.joinable())
        {
            listening = false;
            listener.join();
        }
    }

    void talk(std::atomic<bool> &talking,
              udp_socket socket,
              std::mutex &mutex,
              std::vector<std::pair<udp_sockaddr, std::string>> &sent)
    {
        while (talking)
        {
            std::lock_guard<std::mutex> lock(mutex);
            for (auto &[dst, msg] : sent)
            {
                socket->write(msg, dst);
            }
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }
    }

    void perfect_link::send(const std::string &buf, const udp_sockaddr &dest)
    {
        std::lock_guard<std::mutex> lock(mutex);
        sent.push_back(std::make_pair(dest, buf));
        if (!talking)
        {
            talking = true;
            sender = std::thread(talk, std::ref(talking), socket, std::ref(mutex), std::ref(sent));
        }
    }

    void listen(std::atomic<bool> &listening,
                udp_socket socket,
                std::vector<std::string> &delivered,
                std::vector<std::function<void(std::string &, udp_sockaddr &)>> &callbacks)
    {
        while (listening)
        {
            udp_sockaddr src;
            std::string rec;
            socket->read(rec, 1024, src);
            if (std::find(delivered.begin(), delivered.end(), rec) == delivered.end())
            {
                delivered.push_back(rec);
                for (const auto &deliver : callbacks)
                {
                    deliver(rec, src);
                }
            }
        }
    }

    void perfect_link::upon_deliver(std::function<void(std::string &, udp_sockaddr &)> deliver)
    {
        handlers.push_back(deliver);
        if (!listening)
        {
            listening = true;
            listener = std::thread(listen, std::ref(listening), socket, std::ref(delivered), std::ref(handlers));
        }
    }
}