#pragma once
#include "irc.hpp"

struct s_message {
    std::string _fullStr;
    std::string _prefix;
    std::string _command;
    std::string _params;
};

struct s_flag
{
    char flag;
    int sign; // 1 is +, 2 is -
    bool isValid;
    s_flag *next;
    s_flag *prev;
};

class Channel;

class User {
    public:
        User();
        User(int fd, int id);
        ~User();
        User(User const & src);
        User & operator=(User const & src);

        void set_channel_atm(Channel& channel);
        Channel *get_channel_atm() const;
        int get_fd() const;
        int get_id() const;
        std::string get_nick() const;
    std::string get_host() const;
    std::string get_nickOp() const;
        std::string get_name() const;
        std::string get_pw() const;
        void set_nick(std::string nick);
        void set_nickOp(std::string nick);
        void set_name(std::string name);
        void setOperatorStatus(Channel &channel, bool isOperator);
        bool get_operatorStatus(Channel *channel) const;


        void command_pass(Server &server);
        void command_nick(Server &server, s_message &message);
        void command_user(Server &server, s_message &message);
        void command_topic(Server &server, s_message &message);
        void command_ping(Server &server, s_message &message);
        void command_join(Server &server, s_message &message);
        void command_mode(Server &server, s_message &message);
        void command_privmsg(Server &server, s_message &message);

        void command_part(Server &server, s_message &message);
    s_flag *updateStruct(s_flag *newFlag, int sign, bool isValid);
    s_flag *parserOption(std::string flags);

        void interpretMode(s_flag *parsed, std::vector<std::string> options);
    void splitMessage(int fd, Server& server, std::string buf);
    void parseMessage(Server &server);
    private:

        int _fd;
        int id;
        std::string _nick;
        std::string _nickOp;
        std::string _name;
        std::string _realName;
        std::string _hostName;
        std::string _serverName; // to be deleted ?
        std::string _pw;
        bool _passwordChecked;
        s_message _message;
        Channel *_channel_rn;
        bool _isInAChannel;
        std::map<Channel*, bool> _operatorStatusMap; // TODO : remove in part
};
