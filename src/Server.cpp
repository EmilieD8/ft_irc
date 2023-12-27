/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: edrouot <edrouot@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/24 11:50:47 by mschaub           #+#    #+#             */
/*   Updated: 2023/12/27 10:08:16 by edrouot          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../inc/irc.hpp"

Server::Server(int port, std::string password) {
    _port = port;
    _num_clients = 0;
	_isExit = false;
    _name = "Lit Server";
    _password = password;
    init();
}

Server::~Server() {
}


Server::Server(Server const & src) {
    *this = src;
}

void Server::set_exit_status(bool status) {
    _isExit = status;
}

Server & Server::operator=(Server const & src) {
    if (this != &src) {
        _port = src._port;
        _num_clients = src._num_clients;
        _isExit = src._isExit;
        _channels = src._channels;
        _clients = src._clients;
        _server = src._server;
        _client = src._client;
        _pollfds = src._pollfds;
        _name = src._name;
        _password = src._password;
    }
    return *this;
}

std::string Server::get_password() const {
    return _password;
}

std::vector<User*> &Server::get_clients() {
    return _clients;
}

std::vector<pollfd> * &Server::get_pollfds() {
    return _pollfds;
}
std::vector<Channel *> &Server::get_channels() {
    return _channels;
}

void Server::decrease_num_clients(int i) {
    _num_clients -= i;
}

void Server::init() {
    std::cout << "Initializing..." << std::endl;
    _server.fd = socket(AF_INET, SOCK_STREAM, 0);
    if (_server.fd < 0)
        throw std::runtime_error("Error establishing the socket...");
    fcntl(_server.fd, F_SETFL, O_NONBLOCK);

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
    static int id = 1;
    int new_connection;
    if (!(connectionFds[0].revents & POLLIN))
        return;

    socklen_t size = sizeof(_client.addr);

    new_connection = accept(_server.fd, (struct sockaddr *) &_client.addr, &size);
    if (new_connection == -1)
        throw std::runtime_error("Accepting failed");
    fcntl(new_connection, F_SETFL, O_NONBLOCK);
    if (_num_clients == maxClients)
        throw std::runtime_error("Too many clients");
    connectionFds[_num_clients + 1].fd = new_connection;
    connectionFds[_num_clients + 1].events = POLLIN | POLLOUT;
    User *new_user = new User(new_connection);
    _clients.push_back(new_user);
    _num_clients++;
    id++;
}

void Server::read_client() {
    std::vector<pollfd> &connectionFds = *_pollfds;
    for (int i = 1; i <= _num_clients; i++) {
        if (connectionFds[i].fd != -1 && connectionFds[i].revents & POLLIN) {
            char buf[BUFFER_SIZE + 1];
            std::string fullBuffer;
            int bytes = 0;
            bzero(buf, BUFFER_SIZE + 1);
            while (fullBuffer.find("\r\n") == std::string::npos)
            {
                bytes = read(connectionFds[i].fd, buf, BUFFER_SIZE);
                buf[bytes] = 0;
                fullBuffer.append(buf);
                bzero(buf, BUFFER_SIZE + 1);
            }
            splitBuf(fullBuffer, connectionFds[i].fd, *this);
        }
    }
}


void Server::splitBuf(std::string buf, int fd, Server &server) {
    size_t start = 0;
    size_t end = buf.find("\r\n");
    if (end > BUFFER_SIZE)
        end = BUFFER_SIZE;
    if (end == 0)
        return;
    for (std::vector<User *>::iterator it = _clients.begin(); it != _clients.end(); it++) {

        if ((*it)->get_fd() == fd) {
            while (end != std::string::npos) {
            std::string message(buf.substr(start, end - start));
            start = end + 2;
            end = buf.find("\r\n", start);
            (*it)->splitMessage(server, message);
            }
        break;
        }
    }
}

void Server::launchServer() {
    std::vector<pollfd> &connectionFds = *_pollfds;
    connectionFds[0].fd = _server.fd;
    connectionFds[0].events = POLLIN;
    std::cout << "Launching..." << std::endl;
    while (1)
    {
        try {
            socket_polling();
            connect();
            read_client();
        }
        catch (std::exception &e) {
            std::cerr << "Error: " << e.what() << std::endl;
        }
    }
}
