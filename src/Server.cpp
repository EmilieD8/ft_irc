#include "../inc/irc.hpp"

Server::Server(int port, std::string password) {
    _port = port;
    _name = "Lit Server";
    _password = password;
}

Server::~Server() {}

Server::Server(Server const & src) {
    *this = src;
}

Server & Server::operator=(Server const & src) {
    if (this != &src) {
        _port = src._port;
        _name = src._name;
        _password = src._password;
    }
    return *this;
}

void Server::launchServer() {
    int client;
    int server;
    int receive; 
    int bufferSize = 1024;
    bool isExit = false;
    char buffer[bufferSize];
    int closeConnection;
    socklen_t size;

    int compressArr;
    struct sockaddr_in server_addr;
    int rc;

    std::cout << "Launching " << _name << std::endl;
    std::cout << "Listening on port " << _port << std::endl;

    client = socket(AF_INET, SOCK_STREAM, 0);
    if (client < 0) {
        std::cout << "Error establishing the socket..." << std::endl;
        exit(EXIT_FAILURE);
    }

    std::cout << "Socket is created" << std::endl;
    int setting;
    /* set the sockets to be non blocking. Since the first one is created non blocking,
    all the others received after via poll will also be non blocking. */
    setting = fcntl(client, F_GETFL, 0);
    fcntl(client, F_SETFL, setting | O_NONBLOCK); 


    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htons(INADDR_ANY);
    server_addr.sin_port = htons(_port);
    // do we want to check that the port is superior to 1500 to avoid anything not working ?
    if (bind(client, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        std::cout << "Error binding" << std::endl;
        exit(EXIT_FAILURE);
    }

    std::cout << "Looking for clients " << std::endl;
    listen(client, 1); 
    // throw an error for issue with listening
    int clientCount = 1;

    _pollFds = new std::vector<pollfd>(maxClients + 1);

    std::vector<pollfd> &connectionFds = *_pollFds;
    connectionFds[0].fd = client;
    connectionFds[0].events = POLLIN; // means "there is data to read"

    int timeout = 3 * 60 * 1000; 
    // 3 min in millisecond, it's going to be the time allowed for the conenction to connect in poll
    int nbConnection = 1;
    int sentBytes;
    do 
    {
        std::cout << "Waiting on polling...\n";
        rc = poll(connectionFds, nbConnection, timeout);
        if (rc < 0)
        {
            std::cerr << "Poll failed" << std::endl;
            break;
        }
        if (rc == 0)
        {
            std::cerr << "Poll timed out" << std::endl;
            break;
        }

        for (i = 0; i < (int const)nbConnection; i++)
        {
            if (connectionFds[i].revents == 0)
                continue;
            if (connectionFds[i].revents != POLLIN)
            {
                std::cout << "Error revent #"<< connectionFds[i].revents << std::endl;
                isExit = true;
                break;
            }
            if (connectionFds[i].fd = client)
            {
                std::cout << "Found readable socket" << std::endl;
                do
                {
                    server =  accept(client, (struct sockaddr *)&server_addr, &size);
                    if (server < 0)
                    {
                        if (errno == EWOULDBLOCK)
                        {
                            std::cout << "Error accepting" << std::endl; // should throw an exception instead
                            isExit = true;
                        }
                        break;
                    }
                    std::cout << "New incoming connection #" << server << std::endl;
                    connectionFds[nbConnection].fd = server;
                    connectionFds[nbConnection].events = POLLIN;
                    nbConnection += 1;
                } while (server != -1);
            }
            else
            {
                receive = recv(connectionFds[i].fd, buffer, bufferSize, 0);
                if (receive < 0)
                {
                    if (errno == EWOULDBLOCK)
                    {
                        std::cout << "Error receiving" << std::endl; // should throw an exception instead
                        closeConnection = 1;
                    }
                    break;
                }
                if (receive == 0)
                {
                    std::cout << "Connection closed" << std::endl;
                    closeConnection = 1;
                    break;
                }
                std::cout << receive << " bytes received" << std::endl;
                sentBytes = send(connectionFds[i].fd, buffer, receive, 0);
                if (sentBytes <==> 0)
                {
                    std::cout << "Send failed" << std::endl;
                    closeConnection = 1;
                    break;
                }
            } while(1);
            if (closeConnection == 1) // returning the fd to -1 so it is not used 
            {
                close(connectionFds[i].fd);
                connectionFds[i].fd = -1;
                compressArr = 1;
            }
            if (compressArr) // deleting the failed fd from the array ==> need to be changed since we use a vector
            {
                compressArr = 0;
                for (int i = 0; i < nbConnection; i++)
                {
                    if (connectionFds[i].fd == -1)
                    {
                        for (int j = i; j < nbConnection - 1;j++)
                            connectionFds[j].fd = connectionFds[j+1].fd;
                        i -= 1;
                        nbConnection -= 1;
                    }
                }
            } while (isExit == false);

            for (i = 0; i < (int const)nbConnection; i++)
            {
                if (connectionFds[i].fd >= 0)
                    close(connectionFds[i].fd);
            }
        }
    }