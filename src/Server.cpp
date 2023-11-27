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
//		 }}}

	connectionFds[_num_clients + 1].fd = _client.fd;
    connectionFds[_num_clients + 1].events = POLLIN | POLLOUT;

    // TODO: create a user
    _num_clients++;
}

void Server::read_client()
{
    std::vector<pollfd> &connectionFds = *_pollfds;

    for (int i = 1; i <= _num_clients; i++)
    {
        if (connectionFds[i].fd != -1 && connectionFds[i].revents & POLLIN)
        {
            std::cout << "Reading..." << std::endl;
            char buf[BUFFER_SIZE];
            memset(buf, 0, sizeof(buf));
            int bytes = recv(connectionFds[i].fd, buf, sizeof(buf), 0);
            if (bytes == -1)
            {
                if (errno == EWOULDBLOCK || errno == EAGAIN)
                    continue;
                else
                    throw std::runtime_error("Error reading inside loop");
            }
            else if (bytes == 0) {
                throw std::runtime_error("Error in the reading, bytes == 0");
            }
            splitBuf(buf, connectionFds[i].fd);
        }
    }
}

void Server::splitBuf(std::string buf, int fd)
{
    size_t start = 0;
    size_t end = buf.find("\r\n");

    while (end != std::string::npos)
    {
        Message message(buf.substr(start, end - start));
        start = end + 2;
        end = buf.find("\r\n", start);
        message.splitMessage(fd);
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
          //  send_message();
          if (connectionFds[1].revents & POLLIN)
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
