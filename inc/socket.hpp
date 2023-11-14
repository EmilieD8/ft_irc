#pragma once

#include "irc.hpp"

class Socket
{
    private:
        struct sockaddr_in address;
        int sock;
        int connection;
    public:
        Socket(int domain, int service, int protocol, int port, u_long interface);
        virtual int ConnectToNetwork(int sock, struct sockaddr_in address) = 0;
        void testConnection(int toTest);
        struct sockaddr_in getAddress();
        int getSock();
        int getConnection();

        void setConnection(int connect);
};