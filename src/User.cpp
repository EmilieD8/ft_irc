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
   // std::cout << "nick in get nick " << _nick << std::endl;
    return _nick;
}

std::string User::get_name() const {
    return _name;
}

std::string User::get_pw() const {
    return _pw;
}

void User::set_channel_atm(Channel* channel) {
    _channel_atm = channel;
}

Channel* User::get_channel_atm() const {
    return _channel_atm;
}

void User::set_nick(std::string nick) {
    //std::cout << "Setting nickname to: " << nick << std::endl;
    _nick = nick;
    //std::cout << " nick is now " << _nick << std::endl;
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
    std::string type[] = {"PASS", "NICK", "USER", "JOIN", "KICK", "INVITE", "CHANNEL", "CAP", "PING", "/INVALID" };
    //   TODO : replace also the command by their digit codes
    int count = 0;
    std::cout << "Command : " << _message._command << std::endl;
    //TODO check the right size of the array
    for (int i = 0; i < 9; i++){
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
            command_join(server, this->_message);
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
            command_ping(server, this->_message);
            break;
        case 9:
            std::cout << "Error: invalid command" << std::endl;
            break;
            // TODO : double check
    }
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
    // TODO: maybe create a while loop to give three more tries to connect with the right password
}

bool User::command_nick(Server &server, s_message &message) {
    std::cout << "command_nick function checked" << std::endl;
    // TODO : if user already taken at first instance, then change the username with a special symbol.
    //TODO : check if nickname changes works
    //TODO : chekc if proper error messages sent
    std::string new_nick = message._params;
    std::string old = get_nick();
        if (new_nick.empty()) {
            send(_fd, ERR_NONICKNAMEGIVEN(message._command).c_str(), ERR_NONICKNAMEGIVEN(message._command).size(), 0);
            return false;
        }
        std::vector <User> clients = server.get_clients();
        for (std::vector<User>::iterator it = clients.begin(); it != clients.end(); it++) {
            if (it->get_nick().compare(new_nick) == 0) {
                send(_fd, ERR_NICKNAMEINUSE(message._command, new_nick).c_str(),
                     ERR_NICKNAMEINUSE(message._command, new_nick).size(), 0);
                return false;
            }
        }
        //std::cout << NICK(old, new_nick) << std::endl;
    set_nick(new_nick);
    send(_fd, NICK(old, new_nick).c_str(), NICK(old, new_nick).size(), 0);
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


/*
 * TODO:
 * check if channel already exists or otherwise create it
 * proper name? Test vs #Test
 */

void User::command_join(Server &server, s_message &message) {
    std::cout << "command_join function checked" << std::endl;
    for (std::vector<Channel>::iterator it = server.get_channels().begin(); it != server.get_channels().end(); it++) {
        if (it->get_name() == message._params) {
            it->add_user(*this);
            set_channel_atm(&(*it));
            send(_fd, JOIN(this->get_nick(), this->get_name(), _hostName, it->get_name()).c_str(),
                 JOIN(this->get_nick(), this->get_name(), _hostName, it->get_name()).size(), 0);
        } else {
            Channel *channel = new Channel(message._params, *this);
            //channel->add_user(*this);
            server.get_channels().push_back(*channel);
            set_channel_atm(channel);
            send(_fd, JOIN(this->get_nick(), this->get_name(), _hostName, channel->get_name()).c_str(),
                 JOIN(this->get_nick(), this->get_name(), _hostName, channel->get_name()).size(), 0);
        }
    }
    server.print_channels();
    //send(_fd, RPL_TOPIC(channel->get_name(), channel->get_topic()).c_str(), RPL_TOPIC(channel->get_name(), channel->get_topic()).size(), 0);
}
