#pragma once
#include "irc.hpp"

struct s_socket {
    int fd;
    struct sockaddr_in addr;
};

class User;
class Channel;

class Server {
    public:

        std::vector<User*> &get_clients();
        std::vector<pollfd> * &get_pollfds();
        std::vector<Channel *> &get_channels();
        std::string get_password() const;
        Channel *findChannel(std::string name);
        ~Server();
        void launchServer();
        Server(int port, std::string password);

        void decrease_num_clients(int i);
    private:
        Server(Server const & src);
        Server & operator=(Server const & src);

        int _port;
        int _num_clients;

        std::vector<Channel *> _channels;
        std::vector<User *> _clients;

        s_socket _server;
        s_socket _client;

        std::vector<pollfd> *_pollfds;

        std::string _name;
        std::string _password;

        void init();
        void socket_polling();
        void connect();
        void read_client();
        void send_message();
        void execute(std::string &message);
        void splitBuf(std::string buf, int fd, Server &server);
        pollfd connectionFds[SOMAXCONN];
        const static int maxClients = SOMAXCONN;
};