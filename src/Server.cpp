/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: edrouot <edrouot@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/24 11:50:47 by mschaub           #+#    #+#             */
/*   Updated: 2023/12/19 14:35:49 by edrouot          ###   ########.fr       */
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

void Server::set_exit_status(bool status) {
    isExit = status;
}
Server & Server::operator=(Server const & src) {
    if (this != &src) {
        _port = src._port;
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

    User *new_user = new User(new_connection, id);
    _clients.push_back(new_user);
    _num_clients++;
    id++;
}

void Server::read_client() {
    std::vector<pollfd> &connectionFds = *_pollfds;
    for (int i = 1; i <= _num_clients; i++) {
        if (connectionFds[i].fd != -1 && connectionFds[i].revents & POLLIN) {
            std::cout << "Reading..." << std::endl;
            char buf[BUFFER_SIZE + 1];
            std::string fullBuffer;
            std::cout << "at beg " << fullBuffer << std::endl;
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
//            try {
//                while ((bytes = recv(connectionFds[i].fd, buf, 10, 0)) == 10) {
//                    // bytes = recv(connectionFds[i].fd, buf, sizeof(buf), 0);
//                    //buf[bytes] = 0;
//                    fullBuffer.append(buf, 10);
//                    memset(buf, 0, sizeof(buf));
//                    std::cout << "fullbuffer now: " << fullBuffer << std::endl;
//                }
//                if (bytes == -1) {
//                    if (errno == EWOULDBLOCK)
//                        std::cout << "EWOULDBLOCK" << std::endl;
//                    else
//                        throw std::runtime_error("Error reading inside loop");
//                }
//                else if (bytes > 0 && bytes < 10)
//                {
//                    buf[bytes] = 0;
//                    fullBuffer.append(buf);
//                    std::cout << "fullbuffer end: " << fullBuffer << std::endl;
//                }
//                splitBuf(fullBuffer, connectionFds[i].fd, *this);
//            } catch (const std::exception &e) {
//                std::cerr << e.what() << std::endl;
//            }
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
            (*it)->splitMessage(fd, server, message);
            }
        break;
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
            read_client();
        }
        catch (std::exception &e) {
            std::cerr << "Error: " << e.what() << std::endl;
            exit(EXIT_FAILURE);
        }
    }
}

void Server::print_channels() {
    std::cout << "Printing channels" << std::endl;
    std::vector<Channel *>& temp = get_channels();  // Use a reference

    for (std::vector<Channel *>::iterator channel = temp.begin(); channel != temp.end(); ++channel) {
        std::cout << "Channel name: " << (*channel)->get_name() << std::endl;

        std::vector<User*>& users = (*channel)->get_users();
        if (users.empty()) {
            std::cout << "No users in this channel" << std::endl;
        } else {
            std::cout << "Number of users in this channel: " << users.size() << std::endl;

            for (std::vector<User*>::const_iterator user = users.begin(); user != users.end(); ++user) {
                std::cout << "User nickname: " << (*user)->get_nick() << std::endl;
            }
        }
        std::cout << std::endl;  // Add a separator between channels
    }
}

























/*

void Server::_select() {
    fd_set r, w; //read, write
    int newfd, maxFd = this->_mainFd;
    struct sockaddr_in address;
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(this->_port);
    int clientnum = 0, addrlen = sizeof(address);
    char buf[11];
    size_t bytes;
    std::string tmp;
    while (42)
    {
        signal(SIGINT, _signalCatch);
        signal(SIGQUIT, _signalCatch);
        FD_ZERO(&r);
        FD_ZERO(&w);
        FD_SET(this->_mainFd, &r);
        maxFd = this->_mainFd;
        for (int i = 0; i < clientnum; ++i) {
            maxFd = maxFd this-_clients[i]->getFd() ? this->_clients[i]->getFd() : maxFd;
            FD_SET(this->_clients[i]->getFd(), &r);
            if (!(this->_clients[i]->getWriteBuff().empty())) {
                FD_SET(this->_clients[i]->getFd(), &w);
            }
        }
        if (select(maxFd + 1, &r, &w, 0, 0) == -1){
            _cleaner();
            throw std::runtime_error("irc server select: " + std::string(strerror(errno)));
        }
        if (FD_ISSET(this->_mainFd, &r)){
            if ((newfd = accept(this->_mainFd,(struct sockaddr *) &address,
                                (socklen_t * ) & addrlen)) < 0) {
                std::cout << RED"Accepting new connection failed."RES << std::endl;
            }
            else{
                fcntl(newfd, F_SETFL, O_NONBLOCK);
                this->_clients.push_back(new Client(newfd, this));
                this->_clients[clientnum]->setHost(inet_ntoa(address.sin_addr));
                std::cout << BLUE "Client [" << clientnum << "] "RES << "arrived!" << std::endl;
                clientnum++;
            }
        }
        for (int i = 0; i < clientnum; ++i) {
            if (FD_ISSET(this->_clients[i]->getFd(), &r)){
                while ((bytes = recv(this->_clients[i]->getFd(), buf, 10, 0)) == 10) {
                    buf[bytes] = 0;
                    this->_clients[i]->getReadBuff().append(buf);
                    bzero(buf, 10);
                }
                if (bytes <= 0) {
                    std::cout << RED "Connection to Client[";
                    if (this->_clients[i]->getNickName().empty())
                        std::cout << i;
                    else
                        std::cout << this->_clients[i]->getNickName();
                    std::cout << "] closed" RES << std::endl;
                    close(this->_clients[i]->getFd());
                    deleteClient(this->_clients[i]);
                    clientnum--;
                    break ;
                }
                else if (bytes > 0 && bytes < 10) {
                    buf[bytes] = 0;
                    this->_clients[i]->getReadBuff().append(buf);
                }
                // executing commands
                if (this->_clients[i]->getReadBuff().find('\r') != std::string::npos) {
                    std::istringstream stream(this->_clients[i]->getReadBuff());
                    std::string rb;
                    while (std::getline(stream, tmp)) {
                        rb = this->_clients[i]->getReadBuff();
                        rb = rb.substr(rb.find('\n') + 1);
                        this->_clients[i]->setReadBuff(rb);
                        _printClient(i);
                        this->_clients[i]->callExecute(this->_clients[i]->cmdTokens(
                                tmp));
                    }
                    continue;
                }
            }
            if (FD_ISSET(this->_clients[i]->getFd(), &w)){
                bytes = send(this->_clients[i]->getFd(), this->_clients[i]->getWriteBuff().c_str(),
                             this->_clients[i]->getWriteBuff().size(), 0);
                std::string wb;
                if (bytes == this->_clients[i]->getWriteBuff().size()) {
                    this->_clients[i]->getWriteBuff().clear();
                    this->_clients[i]->getReadBuff().clear();
                }
                else if (bytes != this->_clients[i]->getWriteBuff().size() && bytes > 0) {
                    wb = this->_clients[i]->getWriteBuff();
                    wb = wb.substr(wb[bytes]);
                    this->_clients[i]->setWriteBuff(wb);
                }
                else if (bytes <= 0) {
                    close(this->_clients[i]->getFd());
                    deleteClient(this->_clients[i]);
                    std::cout << RED "Connection to Client["<< i << "] closed" RES << std::endl;
                    clientnum--;
                    break ;
                }
            }
        }
    }
}
*/