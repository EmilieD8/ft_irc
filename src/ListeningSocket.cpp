#include "ListeningSocket.hpp"

Listening::Listening(int domain, int service, int protocol, int port, u_long interface, int backlg) : Binding(domain, service, protocal, port, interface)
{
    backlog = backlg;
    startListening();
    testConnection(listening);
}

void Listening::startListening()
{
    listening = listen(getConnection(), backlog);
}