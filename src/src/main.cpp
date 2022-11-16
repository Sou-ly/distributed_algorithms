#include <chrono>
#include <iostream>
#include <thread>
#include <signal.h>

#include "socket.hpp"
#include "parser.hpp"
#include "hello.h"
#include "perfectlink.hpp"
#include "util.hpp"
#include "urb.hpp"

std::string output_path;
std::map<da::address, std::string> proc_id;
std::vector<std::pair<std::string, da::address>> delivered;
std::vector<std::string> sent;

static void stop(int)
{
  // reset signal handlers to default
  signal(SIGTERM, SIG_DFL);
  signal(SIGINT, SIG_DFL);

  // immediately stop network packet processing
  std::cout << "Immediately stopping network packet processing.\n";

  // write/flush output file if necessary
  std::cout << "Writing output.\n";
  std::fstream output(output_path, std::ios::out);
  for (auto &m : sent)
  {
    output << "b " << m << "\n";
  }

  for (auto &[m, p] : delivered)
  {
    output << "d " << proc_id[p] << " " << m << "\n";
  }

  output.close();

  // exit directly from signal handler
  exit(0);
}

int main(int argc, char **argv)
{
  signal(SIGTERM, stop);
  signal(SIGINT, stop);

  // `true` means that a config file is required.
  // Call with `false` if no config file is necessary.
  bool requireConfig = true;

  Parser parser(argc, argv);
  parser.parse();

  hello();
  std::cout << std::endl;

  std::cout << "My PID: " << getpid() << "\n";
  std::cout << "From a new terminal type `kill -SIGINT " << getpid() << "` or `kill -SIGTERM "
            << getpid() << "` to stop processing packets\n\n";

  std::cout << "My ID: " << parser.id() << "\n\n";

  std::cout << "List of resolved hosts is:\n";
  std::cout << "==========================\n";
  auto hosts = parser.hosts();
  for (auto &host : hosts)
  {
    std::cout << host.id << "\n";
    std::cout << "Human-readable IP: " << host.ipReadable() << "\n";
    std::cout << "Machine-readable IP: " << host.ip << "\n";
    std::cout << "Human-readbale Port: " << host.portReadable() << "\n";
    std::cout << "Machine-readbale Port: " << host.port << "\n";
    std::cout << "\n";
  }
  std::cout << "\n";

  std::cout << "Path to output:\n";
  std::cout << "===============\n";
  std::cout << parser.outputPath() << "\n\n";

  std::cout << "Path to config:\n";
  std::cout << "===============\n";
  std::cout << parser.configPath() << "\n\n";

  std::cout << "Doing some initialization...\n\n";

  // init addr / id pairs
  std::vector<da::address> peers = {};
  for (auto const &peer : parser.hosts())
  {
    da::address remote{peer.ip, peer.port};
    peers.push_back(remote);
    proc_id[remote] = std::to_string(peer.id);
  }

  // read config file
  std::fstream conf(parser.configPath(), std::ios::in);
  unsigned long nbr_msg;
  conf >> nbr_msg;
  conf.close();

  std::cout << "nbr: " << nbr_msg << "\n";

  // save file output path
  output_path = parser.outputPath();
  std::cout << "output path: " << output_path << "\n";

  Parser::Host me = parser.hosts()[parser.id() - 1];
  da::address host{me.ip, me.port};

  da::udp_socket socket = da::socket_descriptor::bind(host);
  da::uniform_reliable_broadcast urb(host, socket, peers);

  std::cout << "Broadcasting...\n\n";
  // broadcasting
  // listening
  urb.on_receive([&](std::string &msg, da::address &src) -> void
                 { delivered.push_back(std::make_pair(msg, src)); 
                   std::cout << "d " << proc_id[src] << " " << msg << "\n"; });
  
  for (unsigned long i = 1; i <= nbr_msg; i++)
  {
    std::string msg = std::to_string(i);
    urb.broadcast(msg);
    sent.push_back(msg);
  }

  while (true)
  {
    std::this_thread::sleep_for(std::chrono::seconds(1));
  }

  return 0;
}