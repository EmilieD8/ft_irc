/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mschaub <mschaub@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/24 11:50:47 by mschaub           #+#    #+#             */
/*   Updated: 2023/11/24 14:44:02 by mschaub          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../inc/irc.hpp"

Server::Server(int port, std::string password) {
    _port = port;
    _name = "Lit Server";
    _password = password;
	isExit = false;
    init();
}

Server::~Server() {
    std::cout << "Closing server" << std::endl;
    close(_server.fd);
}

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

void Server::init() {
    std::cout << "Initializing..." << std::endl;
    _server.fd = socket(AF_INET, SOCK_STREAM, 0);
    if (_server.fd < 0)
        throw std::runtime_error("Error establishing the socket...");
    int setting = fcntl(_server.fd, F_GETFL, 0);
    fcntl(_server.fd, F_SETFL, setting | O_NONBLOCK);

    _server.addr.sin_family = AF_INET;
    if (_port < 1500)
        throw std::runtime_error("Invalid port number, should be > 1500");
    _server.addr.sin_port = htons(_port);
    _server.addr.sin_addr.s_addr = htonl(INADDR_ANY);

    std::cout << "Binding..." << std::endl;
    if (bind(_server.fd, (struct sockaddr *) &_server.addr, sizeof(_server.addr)) < 0)
        throw std::runtime_error("Error binding");

    std::cout << "Listening..." << std::endl;
    if (listen(_server.fd, SOMAXCONN) < 0)
        throw std::runtime_error("Error listening");

    _pollfds = new std::vector<pollfd>(maxClients + 1);
    std::cout << "Initialized" << std::endl;
}

void Server::socket_polling() {
    std::vector<pollfd> &connectionFds = *_pollfds;

    int num_ready = poll(connectionFds.data(), _num_clients + 1, -1);
    if (num_ready < 0)
        throw std::runtime_error("Error polling");

}

void Server::connect() {
    std::vector<pollfd> &connectionFds = *_pollfds;
    if (!(connectionFds[0].revents & POLLIN))
        return;

    socklen_t size = sizeof(_client.addr);

    _client.fd = accept(_server.fd, (struct sockaddr *) &_client.addr, &size);
    if (_client.fd == -1)
        throw std::runtime_error("Accepting failed");

    int setting = fcntl(_client.fd, F_GETFL, 0);
    fcntl(_client.fd, F_SETFL, setting | O_NONBLOCK);

    if (_num_clients == maxClients)
        throw std::runtime_error("Too many clients");

//	char buffer[4096];
//	int receive = recv(_client.fd, buffer, 4096, 0);
//	if (receive < 0)
//		throw std::runtime_error("Receive error");
//	std::istringstream iss(buffer);
//	std::string line;
//	while (std::getline(iss, line)) {
//		std::cout << " line is :" << line << std::endl;
//		if (line.compare(0, 5, "PASS ") == 0) {
//			std::string password = line.substr(5, line.size() - 6);
//			if (password.compare(_password) != 0) {
//				throw std::runtime_error("Error wrong password");
//				isExit = true;
//				break;
//		 }
//	 }
	connectionFds[_num_clients + 1].fd = _client.fd;
    connectionFds[_num_clients + 1].events = POLLIN | POLLOUT;

    // TODO: create a user
    _num_clients++;
}

void Server::read_client()
{
    std::vector<pollfd> &connectionFds = *_pollfds;
    static std::string buffer;

    for (int i = 1; i <= _num_clients; i++)
    {
        if (connectionFds[i].fd != -1 && connectionFds[i].revents & POLLIN)
        {
            std::cout << "Reading..." << std::endl;
            char buf[4096];
            memset(buf, 0, sizeof(buf));
            int bytes = recv(connectionFds[i].fd, buf, sizeof(buf), 0);
            if (bytes > 0) {
                std::string data(buf, bytes);
                std::cout << "Received: " << data << std::endl;

                /* Tried to answer pong when PING is entered, not sure if answer is in correct format */
                if (data.compare(0, 4, "PING") == 0)
                {
                    std::cout << "PING received" << std::endl;
                    // Extract the parameter from the PING message
                    std::string pingParameter = data.substr(5);

                    // Respond with a PONG message
                    std::string pongMessage = "PONG :" + pingParameter + "\r\n";
                    send(connectionFds[i].fd, pongMessage.c_str(), pongMessage.size(), 0);
                }
            }
            if (bytes == -1)
            {
                if (errno == EWOULDBLOCK || errno == EAGAIN) {
                    // No data available, continue to the next client
                    continue;
                }
                else
                    throw std::runtime_error("Error reading inside loop");
            }
            if (bytes == 0)
                throw std::runtime_error("Error in the reading, bytes == 0");

            buffer += buf;
            execute(buffer);
        }
    }
}

void Server::launchServer() {
    std::vector<pollfd> &connectionFds = *_pollfds;
    connectionFds[0].fd = _server.fd;
    connectionFds[0].events = POLLIN;

    _num_clients = 0;

    std::cout << "Launching..." << std::endl;
    while (isExit == false) {
		try {
            socket_polling();
            connect();
            send_message();
            read_client();
        }
        catch (std::exception &e) {
            std::cerr << "Error: " << e.what() << std::endl;
            exit(EXIT_FAILURE);
        }
    }
}

/* That was in mcombeaus repo, not sure what it does... */
void Server::send_message() {
    std::vector<pollfd> &connectionFds = *_pollfds;

    for (std::vector<s_message>::iterator it = _messages.begin(); it != _messages.end(); it++) {
        const s_message &message = *it;
        int index = -1;
        for (int i = 1; i <= _num_clients; i++) {
            if (connectionFds[i].fd == message.fd) {
                index = i;
                break;
            }
        }
        if (index == -1)
            throw std::runtime_error("Error sending message");
    }
}

/* Function to execute the command, atm just splitting it but not doing anything */
void Server::execute(std::string &message) {
    size_t pos = message.find("\r\n");
    if (pos == std::string::npos)
        return;
    std::string command = message.substr(0, pos);
    //std::cout << "Command: " << command << std::endl;
    message.erase(0, pos + 2);
}


//void Server::launchServer() {
//    int client;
//    int server;
//    int receive;
//    int bufferSize = 1024;
//    bool isExit = false;
//    char buffer[bufferSize];
//    int closeConnection;
//    socklen_t size;
//
//    int compressArr;
//    struct sockaddr_in server_addr;
//    int rc;
//
//    std::cout << "Launching " << _name << std::endl;
//    std::cout << "Listening on port " << _port << std::endl;
//
//    client = socket(AF_INET, SOCK_STREAM, 0);
//    if (client < 0) {
//        std::cout << "Error establishing the socket..." << std::endl;
//        exit(EXIT_FAILURE);
//    }
//
//    std::cout << "Socket is created" << std::endl;
//    int setting;
//    /* set the sockets to be non blocking. Since the first one is created non blocking,
//    all the others received after via poll will also be non blocking. */
//    setting = fcntl(client, F_GETFL, 0);
//    fcntl(client, F_SETFL, setting | O_NONBLOCK);
//
//
//    server_addr.sin_family = AF_INET;
//    server_addr.sin_addr.s_addr = htons(INADDR_ANY);
//    server_addr.sin_port = htons(_port);
//    // do we want to check that the port is superior to 1500 to avoid anything not working ?
//    if (bind(client, (struct sockaddr *) &server_addr, sizeof(server_addr)) < 0) {
//        std::cout << "Error binding" << std::endl;
//        exit(EXIT_FAILURE);
//    }
//
//    std::cout << "Looking for clients " << std::endl;
//    listen(client, 1);
//    // throw an error for issue with listening
//    int clientCount = 1;
//
//    //_pollFds = new std::vector<pollfd>(maxClients + 1);
//
//    //std::vector<pollfd> &connectionFds = *_pollFds;
//    connectionFds[0].fd = client;
//    connectionFds[0].events = POLLIN; // means "there is data to read"
//
//    int timeout = 3 * 60 * 1000;
//    // 3 min in millisecond, it's going to be the time allowed for the connection to connect in poll
//    int nbConnection = 1;
//    int sentBytes;
//    do {
//        std::cout << "Waiting on polling...\n";
//        rc = poll(connectionFds, nbConnection, timeout);
//        if (rc < 0) {
//            std::cerr << "Poll failed" << std::endl;
//            break;
//        }
//        if (rc == 0) {
//            std::cerr << "Poll timed out" << std::endl;
//            break;
//        }
//
//        for (int i = 0; i < nbConnection; i++) {
//            if (connectionFds[i].revents == 0)
//                continue;
//            if (connectionFds[i].revents != POLLIN) {
//                std::cout << "Error revent #" << connectionFds[i].revents << std::endl;
//                isExit = true;
//                break;
//            }
//            if (connectionFds[i].fd == client) {
//                std::cout << "Found readable socket" << std::endl;
//                do {
//                    server = accept(client, (struct sockaddr *) &server_addr, &size);
//                    if (server < 0) {
//                        if (errno == EWOULDBLOCK) {
//                            std::cout << "Error accepting" << std::endl; // should throw an exception instead
//                            isExit = true;
//                        }
//                        std::cout << "Error found" << std::endl;
//                        break;
//                    }
//                    std::cout << "New incoming connection #" << server << std::endl;
//                    connectionFds[nbConnection].fd = server;
//                    connectionFds[nbConnection].events = POLLIN;
//                    nbConnection += 1;
//                } while (server != -1);
//            } else {
//                receive = recv(connectionFds[i].fd, buffer, bufferSize, 0);
//                if (receive < 0) {
//                    if (errno == EWOULDBLOCK) {
//                        std::cout << "Error receiving" << std::endl; // should throw an exception instead
//                        closeConnection = 1;
//                    }
//                    break;
//                }
//                if (receive == 0) {
//                    std::cout << "Connection closed" << std::endl;
//                    closeConnection = 1;
//                    break;
//                }
//                std::cout << receive << " bytes received" << std::endl;
//                sentBytes = send(connectionFds[i].fd, buffer, receive, 0);
//                if (sentBytes < 0) {
//                    std::cout << "Send failed" << std::endl;
//                    closeConnection = 1;
//                    break;
//                }
//            }
//            while (1);
//            if (closeConnection == 1) // returning the fd to -1 so it is not used
//            {
//                close(connectionFds[i].fd);
//                connectionFds[i].fd = -1;
//                compressArr = 1;
//            }
//            if (compressArr) // deleting the failed fd from the array ==> need to be changed since we use a vector
//            {
//                compressArr = 0;
//                for (int i = 0; i < nbConnection; i++) {
//                    if (connectionFds[i].fd == -1) {
//                        for (int j = i; j < nbConnection - 1; j++)
//                            connectionFds[j].fd = connectionFds[j + 1].fd;
//                        i -= 1;
//                        nbConnection -= 1;
//                    }
//                }
//            }
//            while (isExit == false);
//
//            for (i = 0; i < (int const) nbConnection; i++) {
//                if (connectionFds[i].fd >= 0)
//                    close(connectionFds[i].fd);
//            }
//        }
//    } while (1);
//}