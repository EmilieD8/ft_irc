#pragma once
#include "irc.hpp"

struct s_message {
    std::string _fullStr;
    std::string _prefix;
    std::string _command;
    std::vector<std::string> _paramsSplit;
    std::string _params;
};

struct s_flag
{
    char flag;
    int sign; // 1 is +, 2 is -
    bool isValid;
    s_flag *next;
    s_flag *prev;
    std::string option;
};

class Channel;

class User {
    public:

        void set_channel_atm(Channel& channel);
        Channel *get_channel_atm() const;
        int get_fd() const;
        int get_id() const;
        std::string get_nick() const;
        std::string get_host() const;
        std::string get_name() const;
        void set_nick(std::string nick);
        void set_name(std::string name);
        void setOperatorStatus(Channel &channel, bool isOperator);
        bool get_operatorStatus(Channel *channel) const;
        void setInviteStatus(Channel &channel, bool isOperator);
        bool get_InviteStatus(Channel *channel) const;

        ~User();
        User(int fd);
        void splitMessage(Server& server, std::string buf);
        void parseMessage(Server &server);
        void send_to(std::string text) const;
        void command_quit(Server &server);
    private:
        User();
        User(User const & src);
        User & operator=(User const & src);

        void command_pass(Server &server);
        void command_nick(Server &server);
        void command_user();
        void command_topic(Server &server);
        void command_ping();
        void command_join(Server &server);
        void command_mode(Server &server);
        void command_privmsg(Server &server);
        void command_part(Server &server);
        void command_kick();
        void command_invite(Server &server);

        s_flag *updateStruct(s_flag *newFlag, int sign, bool isValid);
        bool checkParsing(s_flag *parsed, std::vector<std::string> options);
        void interpretMode(s_flag *parsed, std::vector<std::string> options, Channel &channel);
        s_flag *parserOption(std::string flags);

        int _fd;
        std::string _nick;
        std::string _name;
        std::string _realName;
        std::string _hostName;
        std::string _serverName;
        std::string _color;
        bool _passwordChecked;
        s_message _message;
        Channel *_channel_rn;
        bool _isInAChannel;
        std::map<Channel*, bool> _operatorStatusMap;
        std::map<Channel*, bool> _isInvitedToChannel;
};
