#pragma once
#include "irc.hpp"

class Server {
    public:
        Server(int port, std::string password);
        ~Server();
        Server(Server const & src);
        Server & operator=(Server const & src);
        void launchServer();
    private:
        int _port;
        std::string _name;
        std::string _password;
        std::vector <pollfd> *_pollFds;
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