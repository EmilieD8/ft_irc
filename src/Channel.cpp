#include "../inc/Channel.hpp"

Channel::Channel(std::string name): _name(name) {
    _isInviteOnly = false;
    _topicRestricted = false;
    _keySet = false;
    _limitSet = false;
}

Channel::~Channel() {
   // delete this; 
}

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
    std::cout << "user added to the channel : " << user.get_name() << std::endl;
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

bool Channel::get_topicRestricted() const {
    return _topicRestricted;
}

void Channel::set_topicRestricted(bool topicRestricted) {
    _topicRestricted = topicRestricted;
}

int Channel::get_userSize() const {
    return _users.size();
}

bool Channel::get_limitSet() const {
    return _limitSet;
}

int Channel::get_limit() const {
    return _limit;
}