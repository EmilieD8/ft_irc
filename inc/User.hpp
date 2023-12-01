#pragma once
#include "irc.hpp"

struct s_message {
    std::string _fullStr;
    std::string _prefix;
    std::string _command;
    std::string _params;
};

class User {
    public:
        User();
        User(int fd, int id);
        ~User();
        User(User const & src);
        User & operator=(User const & src);

        int get_fd() const;
        int get_id() const;
        std::string get_nick() const;
        std::string get_name() const;
        std::string get_pw() const;

        bool command_nick(Server &server, s_message &message);
        bool command_user(Server &server, s_message &message);
        void command_ping(Server &server, s_message &message);
        void splitMessage(int fd, Server& server, std::string buf);
        void parseMessage(Server &server);
        void passwordCheck(Server &server);

    private:
        bool isOperator;
        int _fd;
        int id;
        std::string _nick;
        std::string _name;
        std::string _realName;
        std::string _hostName; // might not be necessary
        std::string _serverName; // might not be necessary
        std::string _pw;
        s_message _message;
};
