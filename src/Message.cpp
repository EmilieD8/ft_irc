#include "../inc/irc.hpp"

Message::Message(void)
{
    return;
}
Message::Message(std::string str) : _fullStr(str)
{
    return;
}

Message::~Message(void)
{
    return;
}

Message::Message(Message const &src)
{
    *this = src;
}

Message &Message::operator=(Message const & src) {
    if (this != &src) {
        _fullStr = src._fullStr;
        _prefix = src._prefix;
        _command = src._command;
        _params = src._params;
    }
    return *this;
}

void Message::execMessage(std::string str)
{
    std::cout << "In Message : "<< str << std::endl;
    // divide the messages by space, be sure to retrieve the whole message. 
    //
}
