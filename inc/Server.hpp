#pragma once
#include "irc.hpp"

struct s_socket {
    int fd;
    struct sockaddr_in addr;
};

struct s_message {
    std::string message;
    int fd;
};

class Server {
    public:
        Server(int port, std::string password);
        ~Server();
        Server(Server const & src);
        Server & operator=(Server const & src);
        void launchServer();
    private:
        int _port;
        int _num_clients;
        bool isExit;

        std::vector<s_message> _messages;

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


        //pollfd _pollfds[SOMAXCONN];
        pollfd connectionFds[SOMAXCONN];
        /* it's a certain type of vector, type pollfd and
        it contains this struct 
        struct pollfd 
        {
               int   fd;         file descriptor
               short events;    requested events
               short revents;    returned events
        };*/
        const static int maxClients = SOMAXCONN;

};