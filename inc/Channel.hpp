#pragma once
#include "irc.hpp"

class User;

class Channel {
    public:
        std::string get_topic() const;
        void set_topic(std::string topic);

        std::string get_name() const;
        void set_name(std::string name);

        bool get_topicRestricted() const;
        void set_topicRestricted(bool topicRestricted);

        bool get_inviteOnly() const;
        void set_inviteOnly(bool isInviteOnly);

        bool get_limitSet() const;
        void set_limitSet(bool limitSet);

        int get_limit() const;
        void set_limit(int limit);

        std::string get_password() const;
        void set_password(std::string password);

        bool get_keySet() const;
        void set_keySet(bool key);
        
        std::vector<User*> &get_users();
        int get_userSize() const;

        ~Channel();
        User *findUser(std::string name);
        Channel(std::string name);
        void send_to_all_private(std::string msg, User *user, std::string sender);
        void send_to_all_macro(std::string macro, bool sendToSender, User *user);
        void add_user(User &user);
        void remove_user(User &user);
    private:
        Channel();
        Channel(Channel const & src);
        Channel & operator=(Channel const & src);
        std::string _name;
        std::string _topic;
        std::vector<User*> _users;
        bool _isInviteOnly;
        bool _topicRestricted;
        bool _keySet;
        std::string _password;
        bool _limitSet;
        int _limit;
};