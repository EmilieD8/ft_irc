#pragma once
#include "irc.hpp"

class User;

class Channel {
    public:
        Channel(std::string name);
        Channel(std::string name, User &user);
        ~Channel();
        Channel(Channel const & src);
        Channel & operator=(Channel const & src);
        void add_user(User &user);
        void remove_user(User &user);
        void set_name(std::string name);
        void set_topic(std::string topic);
        std::string get_name() const;
        std::string get_topic() const;
        std::vector<User> &get_users() ;
    private:
        std::string _name;
        std::string _topic;
        std::vector<User> _users;
};