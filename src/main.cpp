#include "../inc/irc.hpp"

int main(int argc, char **argv)
{
    if (argc != 3) {
        std::cerr << "Usage: ./ircserv  <port> <password>" << std::endl;
        exit(EXIT_FAILURE);
    }
    try {
        int port = std::stoi(argv[1]);
        std::string password = argv[2];
        Server server(port, password);
        server.launchServer();
    }
    catch (std::exception &e) {
        std::cerr << "Error: " << e.what() << std::endl;
        exit(EXIT_FAILURE);
    }
    return 0;
}