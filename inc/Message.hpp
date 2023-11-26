#include "irc.hpp"

class Message {
    public:
        Message();
        Message(const std::string);
        ~Message();
        Message(Message const & src);
        Message & operator=(Message const & src);
        static void execMessage(std::string str);
    private:
        std::string _prefix;
        std::string _command;
        std::string _params;
        std::string _fullStr;
};