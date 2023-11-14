#pragma once

#include "irc.hpp"

class Socket
{
    private:
        struct sockaddr_in address;
        int sock;
    public:
        Socket(int domain, int service, int protocol, int port, u_long interface);
       
        virtual void ConnectToNetwork(int sock, struct sockaddr_in address) = 0;
        void testConnection(int toTest);

        struct sockaddr_in getAddress();
        int getSock();
};