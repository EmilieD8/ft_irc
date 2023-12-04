#include "../inc/Channel.hpp"


Channel::Channel(std::string name): _name(name) {}

Channel::Channel(std::string name, User &user): _name(name)  {
    _users.push_back(user);
}

Channel::~Channel() {}

Channel::Channel(Channel const &src) {
    *this = src;
}

Channel &Channel::operator=(Channel const &src) {
    if (this != &src) {
        _name = src._name;
        _topic = src._topic;
        _users = src._users;
    }
    return *this;
}

void Channel::add_user(User &user) {
    _users.push_back(user);
}

void Channel::remove_user(User &user) {
    for (std::vector<User>::iterator it = _users.begin(); it != _users.end(); ++it) {
        if (it->get_fd() == user.get_fd()) {
            _users.erase(it);
            break;
        }
    }
}

void Channel::set_name(std::string name) {
    _name = name;
}

void Channel::set_topic(std::string topic) {
    _topic = topic;
}

std::string Channel::get_name() const {
    return _name;
}

std::string Channel::get_topic() const {
    return _topic;
}

std::vector<User> &Channel::get_users()  {
    return _users;
}