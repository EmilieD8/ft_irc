#include "../inc/irc.hpp"
#include <signal.h>

Server* g_server = NULL;

void clear_all(Server *server) {
    for (std::vector<Channel*>::iterator it = server->get_channels().begin(); it != server->get_channels().end(); it++) {
        delete *it;
    }
    server->get_channels().clear();
    for (std::vector<User*>::iterator it = server->get_clients().begin(); it != server->get_clients().end(); it++) {
        delete *it;
    }
    server->get_clients().clear();
    for (std::vector<pollfd>::iterator it = server->get_pollfds()->begin(); it != server->get_pollfds()->end(); it++) {
        close(it->fd);
    }
    server->get_pollfds()->clear();
    delete server->get_pollfds();
}

void signal_handler(int sig) {
    if (sig == SIGINT)
    {
        std::cout << "end of program" << std::endl;
        if (g_server)
        {
            clear_all(g_server);
            delete g_server;
        }
        exit(EXIT_SUCCESS);
    }
}

int main(int argc, char **argv)
{
    if (argc != 3) {
        std::cerr << "Usage: ./ircserv  <port> <password>" << std::endl;
        exit(EXIT_FAILURE);
    }
    for (int i = 0; argv[1][i] != '\0'; i++) {
        if (!std::isdigit(argv[1][i])) {
            std::cerr << "Port must be a number" << std::endl;
            exit(EXIT_FAILURE);
        }
    }
    int port = std::atoi(argv[1]);
    std::string password = argv[2];
    signal(SIGINT, signal_handler);
    try
    {
        g_server = new Server(port, password);
        g_server->launchServer();
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }
    return 0;
}


