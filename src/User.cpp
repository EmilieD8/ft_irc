#include "../inc/irc.hpp"

User::User() {}

User::User(int fd, int id) : _fd(fd), id(id), isOperator(false) {}

User::~User() {}

User::User(User const &src) {
    *this = src;
}

User &User::operator=(User const &src) {
    if (this != &src) {
        _fd = src._fd;
        id = src.id;
        isOperator = src.isOperator;
    }
    return *this;
}

int User::get_fd() const {
    return _fd;
}

int User::get_id() const {
    return id;
}

std::string User::get_nick() const {
    return _nick;
}

std::string User::get_name() const {
    return _name;
}

std::string User::get_pw() const {
    return _pw;
}


void User::splitMessage(int fd, Server &server, std::string buf) {
    std::stringstream ss(buf);
    std::string word;
    int count;

    if (buf[0] == ':')
        count = 0;
    else
        count = 1;
    while (ss >> word) {
        if (count == 0)
            _message._prefix = word;
        else if (count == 1)
            _message._command = word;
        else if (count == 2)
            _message._params = word;
        else
            _message._params += " " + word;
        count++;
    }
//    std::cout << "Prefix : " << _message._prefix << std::endl;
//    std::cout << "Command : " << _message._command << std::endl;
//    std::cout << "Params : " << _message._params << std::endl;

    parseMessage(server);
}

void User::parseMessage(Server &server) {
    std::string type[] = {"PASS", "NICK", "USER", "/JOIN", "/KICK", "/INVITE", "/CHANNEL", "/CAP", "/PASS", "/NICK", "/USER", "PING","CAP", "/INVALID" };
    //   TODO : replace also the command by their digit codes
    int count = 0;
    std::cout << "Command : " << _message._command << std::endl;
    //TODO check the right size of the array
    for (int i = 0; i < 13; i++){
        if (_message._command.compare(type[i]) != 0)
            count++;
        else
            break;
    }
    std::cout << "count : " << count << std::endl;
    switch (count) {
        case 0:
            passwordCheck(server);
            break;
        case 1:
            command_nick(server, this->_message);
            break;
        case 2:
            command_user(server, this->_message);
            //TODO check if we need bool or not and check both nickname and user name
            break;
        case 3:
            //TODO JOIN
            break;
        case 4:
            //TODO KICK
            break;
        case 5:
            //TODO INVITE
            break;
        case 6:
            //TODO CHANNEL
            break;
        case 7:
            //TODO CAP
            break;
        case 8:
            //TODO /PASS
            break;
        case 9:
            //TODO /NICK
            break;
        case 10:
            //TODO /USER
            break;
        case 11:
            command_ping(server, this->_message);
        case 12:
            break;
        case 13:
            std::cout << "Error: invalid command" << std::endl; // TODO : double check
    }
    //TODO
}

void User::passwordCheck(Server &server) {
    std::cout << "password function checked" << std::endl;

    if (_message._params.compare(server.get_password()) != 0) {
        send(_fd, ERR_PASSWDMISMATCH, 26, 0);
        server.set_exit_status(true);
        throw std::runtime_error("Error wrong password");
    }
    else
        std::cout << "password is correct" << std::endl;
    // maybe create a while loop to give three more tries to connect with the right password
}

bool User::command_nick(Server &server, s_message &message) {
    std::cout << "command_nick function checked" << std::endl;
    // nick has to be unique -> loop through vector list to check
    //
    std::string new_nick = message._params;
   // TODO : check if the nickname is empty
    for (std::vector<User>::iterator it = server.get_clients().begin(); it != server.get_clients().end(); it++) {
        if (it->get_nick().compare(new_nick) == 0) {
            std::cout << "Error: nickname already taken" << std::endl;
            //TODO send error message
            return false;
        }
    }
    _nick = new_nick;
    send(_fd, NICK(_nick, new_nick).c_str(), NICK(_nick, new_nick).size(), 0);
    return true;
}

bool User::command_user(Server &server, s_message &message) {
    std::cout << "command_user function checked" << std::endl;
    //format : <username> <hostname> <servername> :<realname>
    std::stringstream ss(message._params);
    std::string word;
    int count = 0;

    while (ss >> word) {
        if (count == 0)
            this->_name = word;
        else if (count == 1)
            this->_hostName = word;
        else if (count == 2)
            this->_serverName = word;
        else if (count == 3 and word[0] == ':')
            this->_realName = word;
        else if (count == 3 and word[0] != ':')
            break;
        else
            this->_realName += " " + word;
        count++;
    }
    if (count < 4) {
        send(_fd, ERR_NEEDMOREPARAMS(message._command).c_str(), ERR_NEEDMOREPARAMS(message._command).size() , 0);
        return false;
    }
    send(_fd, RPL_WELCOME(_nick, _name, _hostName).c_str(), RPL_WELCOME(_nick, _name, _hostName).size(), 0);
    return true;
}

void User::command_ping(Server &server, s_message &message) {
    std::cout << "command_ping function checked" << std::endl;
    if (_message._params.empty()) {
        send(_fd, ERR_NOORIGIN(_message._command).c_str(), ERR_NOORIGIN(_message._command).size(), 0);
        return;
    }
    else
        send(_fd, PONG(_message._params).c_str(), PONG(_message._params).size(), 0);
}