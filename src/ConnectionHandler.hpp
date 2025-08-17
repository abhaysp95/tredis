#ifndef __CONNECTION_HANDLER_HPP_
#define __CONNECTION_HANDLER_HPP_

#include <functional>
#include <netinet/in.h>
#include <thread>

class ConnectionHandler {
  int client_fd;
  struct sockaddr_in client_addr;
  std::thread t;

  using HandlerFunc = std::function<void(int, const struct sockaddr_in &)>;

  // can have logic to handle after some timeout or some client signaling too,
  // in this
  void stop();

public:
  explicit ConnectionHandler(int server_fd, HandlerFunc &&handler);

  ~ConnectionHandler();
};

#endif
