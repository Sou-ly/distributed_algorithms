#include <chrono>
#include <iostream>
#include <thread>
#include <signal.h>

#include "socket.hpp"
#include "parser.hpp"
#include "hello.h"
#include "perfectlink.hpp"

static void stop(int)
{
  // reset signal handlers to default
  signal(SIGTERM, SIG_DFL);
  signal(SIGINT, SIG_DFL);

  // immediately stop network packet processing
  std::cout << "Immediately stopping network packet processing.\n";

  // write/flush output file if necessary
  std::cout << "Writing output.\n";

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

  Parser::Host me = parser.hosts()[parser.id()];
  da::udp_sockaddr host{me.ip, me.port};
  da::udp_socket socket = da::socket_descriptor::bind(host);
  da::perfect_link link(socket);
  link.upon_deliver([](std::string &msg, da::udp_sockaddr &src) -> void
                    { std::cout << msg << "\n"; });

  std::cout << "Broadcasting...\n\n";

  // After a process finishes broadcasting,
  // it waits forever for the delivery of messages.
  std::string message = std::to_string(parser.id());
  for (auto &peer : parser.hosts())
  {
    da::udp_sockaddr host{peer.ip, peer.port};
    link.send(message, host);
  }
  while (true)
  {
    std::this_thread::sleep_for(std::chrono::seconds(1));
  }

  return 0;
}

/**
   Parser::Host me = parser.hosts()[parser.id()];

  // create socket
  int sockfd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
  if (sockfd < 0)
  {
    perror("socket");
    exit(errno);
  }

  // make it reusable when closed
  int optval = 1;
  setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof optval);

  sockaddr_in my;
  memset(&my, 0, sizeof(sockaddr_in));
  my.sin_addr.s_addr = me.ip;
  my.sin_port = me.port;
  my.sin_family = AF_INET;

  // bind socket to host
  if (bind(sockfd, reinterpret_cast<const sockaddr *>(&my), sizeof(sockaddr_in)) < 0)
  {
    perror("bind");
    exit(errno);
  }

  PerfectLink link(sockfd);

  flp2p::Deliver deliver = flp2p::Deliver([](flp2p::Datagram dgram) -> void
                                          { std::cout << "received: " << reinterpret_cast<const char *>(dgram.content) << "\n"; });

  link.on_receive(deliver);

  std::cout << "Broadcasting and delivering messages...\n\n";

  for (int i = 0; i < 100; i++)
  {
    std::string message = std::to_string(i);
    const char *msg = message.c_str();

    for (auto &peer : parser.hosts())
    {
      flp2p::Datagram dgram;
      dgram.msg_id = i;
      dgram.ip = peer.ip;
      dgram.port = peer.port;
      dgram.content = new char[strlen(msg)];
      strcpy(reinterpret_cast<char *>(dgram.content), msg);
      dgram.len = strlen(msg);

      link.send(dgram);
    }
  }
 *
 */