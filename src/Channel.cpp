#include "../inc/Channel.hpp"

Channel::Channel(std::string name): _name(name) {
    _isInviteOnly = false;
    _topicRestricted = false;
    _keySet = false;
    _limitSet = false;
    _password = "\0";
    _limit = 0;
    _topic = "\0";
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
        // change to private attributes ++
    }
    return *this;
}

void Channel::add_user(User &user) {
    _users.push_back(&user);
}

void Channel::remove_user(User &user) {
    for (std::vector<User *>::iterator it = _users.begin(); it != _users.end(); ++it) {
        if ((*it)->get_fd() == user.get_fd()) {
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

std::vector<User*> &Channel::get_users()  {
    return _users;
}

bool Channel::get_topicRestricted() const {
    return _topicRestricted;
}

void Channel::set_topicRestricted(bool topicRestricted) {
    _topicRestricted = topicRestricted;
}

bool Channel::get_inviteOnly() const {
    return _isInviteOnly;
}

void Channel::set_inviteOnly(bool isInviteOnly) {
    _isInviteOnly = isInviteOnly;
}


int Channel::get_userSize() const {
    return _users.size();
}

bool Channel::get_limitSet() const {
    return _limitSet;
}

void Channel::set_limitSet(bool limitSet) {
    _limitSet = limitSet;
}

int Channel::get_limit() const {
    return _limit;
}

void Channel::set_limit(int limit) {
    _limit = limit;
}

std::string Channel::get_password() const {
    return _password;
}

void Channel::set_password(std::string password) {
    _password = password;
}

bool Channel::get_keySet() const {
    return _keySet;
}

void Channel::set_keySet(bool key) {
    _keySet = key;
}

void Channel::send_to_all_private(std::string msg, User *user, std::string sender) {
 // make sure that _users is not empty and is in channel
    for (std::vector<User *>::iterator it = _users.begin(); it != _users.end(); it++) {
        if (((*it)->get_nick() != user->get_nick() && (*it)->get_nick() != user->get_nickOp()) && (*it)->get_channel_atm()->get_name() == this->get_name())
            send((*it)->get_fd(), PRIVMSG(sender, (*it)->get_name(), (*it)->get_host(), _name, msg).c_str(),
                 PRIVMSG(sender, (*it)->get_name(), (*it)->get_host(), _name, msg).size(), 0 );

    }
}

void Channel::send_to_all_macro(std::string macro) {
    // make sure that _users is not empty
    for (std::vector<User *>::iterator it = _users.begin(); it != _users.end(); it++) {
            send((*it)->get_fd(), macro.c_str(),macro.size(), 0 );
    }
}