#include "perfectlink.hpp"

namespace da
{
    perfect_link::perfect_link(udp_socket socket) : socket(socket)
    {
        talking = false;
        listening = false;
        delivered = {};
        id = 0;
    }

    perfect_link::~perfect_link()
    {
        if (talking)
        {
            talking = false;
            sender.join();
        }
        if (listening)
        {
            listening = false;
            listener.join();
        }
    }

    void talk(std::atomic<bool> &talking,
              udp_socket socket,
              std::mutex &mutex,
              std::vector<std::tuple<udp_sockaddr, unsigned long, void *, size_t>> &sent)
    {
        while (talking)
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
            std::lock_guard<std::mutex> lock(mutex);
            for (auto &[dst, id, msg, size] : sent)
            {
                socket->write(msg, size, dst);
            }
        }
    }

    void perfect_link::send(const std::string &buf, const udp_sockaddr &dest)
    {
        id++;
        // header
        unsigned char *bytes = reinterpret_cast<unsigned char *>(malloc(5 + buf.size()));
        // ack
        bytes[0] = static_cast<unsigned char>(false);
        // sequence number
        bytes[1] = (id >> 24) & 0xFF;
        bytes[2] = (id >> 16) & 0xFF;
        bytes[3] = (id >> 8) & 0xFF;
        bytes[4] = (id >> 0) & 0xFF;
        std::memcpy(&bytes[5], buf.c_str(), buf.size());
        // grab sent lock and push into the queue
        std::lock_guard<std::mutex> lock(mutex);
        sent.push_back(std::make_tuple(dest, id, bytes, 5 + buf.size()));
        if (!talking)
        {
            talking = true;
            sender = std::thread(talk, std::ref(talking), socket, std::ref(mutex), std::ref(sent));
        }
    }

    void listen(std::atomic<bool> &listening,
                udp_socket socket,
                std::mutex &mutex,
                std::map<udp_sockaddr, std::vector<unsigned long>> &delivered,
                std::vector<std::tuple<udp_sockaddr, unsigned long, void *, size_t>> &sent,
                std::vector<std::function<void(std::string &, udp_sockaddr &)>> &callbacks)
    {
        while (listening)
        {
            udp_sockaddr src;
            ssize_t size;
            unsigned char *rec = reinterpret_cast<unsigned char *>(malloc(100));
            if ((size = socket->read(rec, 128, src)) > 0)
            {
                bool ack = static_cast<bool>(rec[0]);
                unsigned long id = (rec[1] << 24) | (rec[2] << 16) | (rec[3] << 8) | (rec[4]);
                if (ack)
                {
                    std::lock_guard<std::mutex> lock(mutex);
                    auto it = std::find_if(sent.begin(),
                                           sent.end(),
                                           [&](std::tuple<udp_sockaddr, unsigned long, void *, size_t> tup) -> bool
                                           {
                                               return std::get<1>(tup) == id;
                                           });
                    if (it != sent.end())
                    {
                        free(std::get<2>(*it));
                        sent.erase(it);
                    }
                }
                else
                {
                    rec[0] = static_cast<unsigned char>(true);
                    socket->write(&rec[0], 5, src);
                    if (std::find(delivered[src].begin(), delivered[src].end(), id) == delivered[src].end())
                    {
                        std::string msg(reinterpret_cast<char *>(&rec[5]));
                        delivered[src].push_back(id);
                        for (const auto &deliver : callbacks)
                        {
                            deliver(msg, src);
                        }
                    }
                }
            }
            free(rec);
        }
    }

    void perfect_link::upon_deliver(std::function<void(std::string &, udp_sockaddr &)> deliver)
    {
        handlers.push_back(deliver);
        if (!listening)
        {
            listening = true;
            listener = std::thread(listen, std::ref(listening), socket, std::ref(mutex), std::ref(delivered), std::ref(sent), std::ref(handlers));
        }
    }
}