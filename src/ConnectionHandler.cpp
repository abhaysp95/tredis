#include "ConnectionHandler.hpp"
#include "./log_utils.hpp"

ConnectionHandler::ConnectionHandler(int server_fd, HandlerFunc &&handler) {

  int client_addr_len = sizeof(this->client_addr);
  spd::info("Waiting for a client to connect...\n");
  this->client_fd = accept(server_fd, (struct sockaddr *)&this->client_addr,
                           (socklen_t *)&client_addr_len);
  if (client_fd < 0) {
    // can this be improved
    throw std::runtime_error("Problem in acception connection!!!");
  }

  this->t = std::thread([this, handler = std::move(handler)] {
    handler(this->client_fd, this->client_addr);
  });
};

void ConnectionHandler::stop() { close(this->client_fd); }

ConnectionHandler::~ConnectionHandler() {
  this->stop();
  if (this->t.joinable())
    this->t.join();
}
