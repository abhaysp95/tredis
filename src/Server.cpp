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
  char command[256];
  bzero(command, 256);

  if (int rbytes = read(client_fd, command, sizeof(command)); -1 != rbytes) {
    spd::info("recieved from client: {}\n", parse_crlf(command));
    if (auto parsed_command = resp_parser(command); parsed_command) {
      spd::info("parsed_command: {}\n", parse_crlf(*parsed_command));
      if ("PING" == *parsed_command) {
        std::string resp = "+PONG\r\n";
        if (int wbytes = write(client_fd, resp.c_str(), resp.size());
            wbytes != resp.size()) {
          spd::error("Command PING response, write error\n");
        }
      }
    } else {
      spd::error("Failure in parsing command: {}\n", parse_crlf(command));
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
  while (true) {
    handlers.emplace_back(
        std::make_unique<ConnectionHandler>(server_fd, resp_command_response));
  }

  return 0;
}
