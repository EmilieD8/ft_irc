#include "Binding.hpp"

Binding::Binding(int domain, int service, int protocol, int port, u_long interface) : Socket(domain, service, protocol, port, interface)
{
    setConnection(bindOrConnect(getSock(), getAddress()));
    testConnection(getConnection());
}
int Binding::bindOrConnect(int sock, struct sockaddr_in address)
{
    return bind(sock, (struct sockaddr *)&address, sizeof(address));
}