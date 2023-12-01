#pragma once
#include "irc.hpp"
#include <vector>



class Message {
    public:
        Message();
        Message(const std::string);
        ~Message();
        Message(Message const & src);
        Message & operator=(Message const & src);
        void splitMessage(int fd, Server& server);
        void parseMessage();
        void passwordCheck();
    private:
        std::string _fullStr;
        std::string _prefix;
        std::string _command;
        std::string _params;

};