#include "./log_utils.hpp"
#include "./resp_parser.hpp"
#include "ConnectionHandler.hpp"
#include <arpa/inet.h>
#include <iostream>
#include <netdb.h>
#include <strings.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

void resp_command_response(int client_fd,
                           const struct sockaddr_in &client_addr) {
  spd::info("Client connected: addr: {}, port: {}\n",
            client_addr.sin_addr.s_addr, client_addr.sin_port);
  // we'll handling streaming logic later
  char command[1024];
  bzero(command, 1024);

  // client can write to connection multiple times, so we need to read multiple
  // times
  while (true) {
    if (int rbytes = read(client_fd, command, sizeof(command)); -1 != rbytes) {
      spd::info("recieved from client: {}\n", parse_crlf(command));
      std::string_view buffer(command);
      size_t pos = 0;
      std::string resp = "+PONG\r\n";
      if ((write(client_fd, resp.c_str(), resp.size())) != resp.size()) {
        spd::error("Error in sending response to client");
      }
    } else {
      spd::error("Error in reading: {}\n");
      close(client_fd);
    }
  }
}

int main(int argc, char **argv) {
  // Flush after every std::cout / std::cerr
  std::cout << std::unitbuf;
  std::cerr << std::unitbuf;

  int server_fd = socket(AF_INET, SOCK_STREAM, 0);
  if (server_fd < 0) {
    std::cerr << "Failed to create server socket\n";
    return 1;
  }

  // Since the tester restarts your program quite often, setting SO_REUSEADDR
  // ensures that we don't run into 'Address already in use' errors
  // Be mindful that socket allows you to work over any lower level as it opens
  // for layer 4
  int reuse = 1;
  if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse)) <
      0) {
    spd::error("setsockopt failed\n");
    return 1;
  }

  struct sockaddr_in server_addr;
  server_addr.sin_family = AF_INET;
  server_addr.sin_addr.s_addr = INADDR_ANY;
  server_addr.sin_port = htons(6379);

  if (bind(server_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) !=
      0) {
    spd::error("Failed to bind to port 6379\n");
    return 1;
  }

  int connection_backlog = 5;
  if (listen(server_fd, connection_backlog) != 0) {
    spd::error("listen failed\n");
    return 1;
  }

  std::vector<std::unique_ptr<ConnectionHandler>> handlers{};
  // while (true) { // single connection for now
  handlers.emplace_back(
      std::make_unique<ConnectionHandler>(server_fd, resp_command_response));
  // }

  return 0;
}
