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
    int bufferSize = 1024;
    bool isExit = false;
    char buffer[bufferSize];
    socklen_t size;
    struct sockaddr_in server_addr;

    std::cout << "Launching " << _name << std::endl;
    std::cout << "Listening on port " << _port << std::endl;

    client = socket(AF_INET, SOCK_STREAM, 0);
    if (client < 0) {
        std::cout << "Error establishing the socket..." << std::endl;
        exit(EXIT_FAILURE);
    }

    std::cout << "Socket is created" << std::endl;

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htons(INADDR_ANY);
    server_addr.sin_port = htons(_port);

    if (bind(client, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        std::cout << "Error binding" << std::endl;
        exit(EXIT_FAILURE);
    }

    std::cout << "Looking for clients " << std::endl;
    listen(client, 1);
    int clientCount = 1;
    size = sizeof(server_addr);
    server = accept(client, (struct sockaddr *)&server_addr, &size);
    if (server < 0)
        std::cout << "Error accepting" << std::endl;

//    while (server > 0)
//    {
//        strcpy(buffer, "Server connected...\n");
//        send(server, buffer, bufferSize, 0);
//        std::cout << "Connected with the client #" << clientCount << " ok" << std::endl;
//        std::cout << "Enter # to end the connection" << std::endl;
//        std::cout << "client:";
//        recv(server, buffer, bufferSize, 0);
//        while (*buffer != '*') {
//            std::cout << buffer << " ";
//            if (*buffer == '#') {
//                *buffer = '*';
//                isExit = true;
//            }
//            recv(server, buffer, bufferSize, 0);
//        }
//
//        while (!isExit) {
//            std::cout << "\nServer: ";
//            while (*buffer != '*') {
//                std::cin >> buffer;
//                send(server, buffer, bufferSize, 0);
//                if (*buffer == '#') {
//                    send(server, buffer, bufferSize, 0);
//                    *buffer = '*';
//                    isExit = true;
//                }
//            }
//            while (*buffer != '*') {
//                std::cout << "Client: ";
//                recv(server, buffer, bufferSize, 0);
//                std::cout << buffer << " ";
//                if (*buffer == '#') {
//                    *buffer = '*';
//                    isExit = true;
//                }
//            }
//        }
//        std::cout << "\n\n=> Connection terminated with IP " << inet_ntoa(server_addr.sin_addr);
//        close(server);
//        std::cout << "\nGoodbye..." << std::endl;
//        isExit = false;
//        exit(1);
//    }
    while (true) {
        int bytesRead = recv(server, buffer, bufferSize, 0);
        if (bytesRead <= 0) {
            std::cout << "Connection closed by client." << std::endl;
            break;
        }

        buffer[bytesRead] = '\0'; // Null-terminate the received data
        std::cout << "Client: " << buffer << std::endl;
        std::cout << buffer << std::endl;

        // You can add additional processing or conditions here as needed.

        // Example: If the received message is "exit", close the connection.
        if (strcmp(buffer, "exit") == 0) {
            std::cout << "Client requested to exit. Closing connection." << std::endl;
            break;
        }
    }
    close(client);
}
