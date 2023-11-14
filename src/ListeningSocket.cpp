#include "../inc/ListeningSocket.hpp"

Listening::Listening(int domain, int service, int protocol, int port, u_long interface, int backlg) : Binding(domain, service, protocol, port, interface)
{
    std::cout << "Here listen" << std::endl;
    backlog = backlg;
    startListening();
    testConnection(listening);
}

void Listening::startListening()
{
    std::cout << getSock() << std::endl;
    listening = listen(getSock(), backlog);
}

int Listening::getListening()
{
    return listening;
}

int Listening::getBacklog()
{
    return backlog;
}