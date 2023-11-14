#pragma once

#include <socket.hpp>
#include "webserv.hpp"

class Binding : public Socket
{
    public:
        Binding(int domain, int service, int protocol, int port, u_long interface) : Socket(domain, service, protocol, port, interface);
        int bindOrConnect(int sock, struct sockaddr_in address);
        


};