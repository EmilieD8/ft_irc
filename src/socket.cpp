#include <socket.hpp>

Socket::Socket(int domain, int service, int protocol, int port, u_long interface)
{
    // Address struct definined --> has to be taken from the config files
    address.sin_family = domain;
    address.sin_port = htons(port);
    address.sin_addr.s_addr = htonl(interface);
    //establishing socket and connection + testing both 
    sock = socket(domain, service, protocol);
    testConnection(sock);
}

void Socket::testConnection(int toTest) // to test the sock or the connection
{
    if (toTest < 0)
    {
        std::cerr << "Failed to connect..." << std::endl;
        exit(EXIT_FAILURE);
    }
}

struct sockaddr_in Socket::getAddress()
{
    return address;
}

int Socket::getSock()
{
    return sock;
}

int Socket::getConnection()
{
    return connection;
}

void Socket::setConnection(int connect)
{
    connection = connect;
}