#pragma once

#include "socket.hpp"
#include "irc.hpp"

class Binding : public Socket
{
    public:
        Binding(int domain, int service, int protocol, int port, u_long interface);
        int ConnectToNetwork(int sock, struct sockaddr_in address);
};