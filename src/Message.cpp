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

void Message::splitMessage(int fd) {
    std::stringstream ss(_fullStr);
    std::string word;
    int count;

    if (_fullStr[0] == ':')
        count = 0;
    else
        count = 1;
    while (ss >> word) {
        if (count == 0)
            _prefix = word;
        else if (count == 1)
            _command = word;
        else if (count == 2)
            _params = word;
        else
            _params += " " + word;
        count++;
    }
//    std::cout << "Prefix : " << _prefix << std::endl;
//    std::cout << "Command : " << _command << std::endl;
//    std::cout << "Params : " << _params << std::endl;

    parseMessage();
}

void Message::parseMessage() {
    std::string type[] = {"PASS", "NICK", "USER", "/JOIN", "/KICK", "/INVITE", "/CHANNEL", "/CAP", "/PASS", "/NICK", "/USER", "/INVALID"};
    //   TODO : replace also the command by their digit codes
    int count = 0;
    //    std::cout << "Command : " << _command << std::endl;

    for (int i = 0; i < 8; i++) {
        if (_command.compare(type[i]) != 0)
            count++;
        else
            break;
    }
    std::cout << count << std::endl;
    switch (count) {
        case 0:
            //TODO PASS
            break;
        case 1:
            //TODO NICK
            break;
        case 2:
            //TODO USER
            break;
        case 3:
            //TODO JOIN
            break;
        case 4:
            //TODO KICK
            break;
        case 5:
            //TODO INVITE
            break;
        case 6:
            //TODO CHANNEL
            break;
        case 7:
            //TODO CAP
            break;
        case 8:
            //TODO /PASS
            break;
        case 9:
            //TODO /NICK
            break;
        case 10:
            //TODO /USER
            break;
        default:
            throw std::runtime_error("Error: invalid command");
    }
    //TODO
};

void Message::pass() {
    //TODO
};