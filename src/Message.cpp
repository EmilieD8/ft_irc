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

    // switch statement to be done. 
    // divide the messages by space, be sure to retrieve the whole message. 
    //
    // /* Tried to answer pong when PING is entered, not sure if answer is in correct format */
        // if (data.compare(0, 4, "PING") == 0)
        // {
        //     std::cout << "PING received" << std::endl;
        //     // Extract the parameter from the PING message
        //     std::string pingParameter = data.substr(5);

        //     // Respond with a PONG message
        //     std::string pongMessage = "PONG :" + pingParameter + "\r\n";
        //     send(connectionFds[i].fd, pongMessage.c_str(), pongMessage.size(), 0);
        // }
}
