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

};