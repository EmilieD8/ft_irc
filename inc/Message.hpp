#include "irc.hpp"

class Message {
    public:
        Message();
        ~Message();
        Message(Message const & src);
        Message & operator=(Message const & src);
    private:
        std::string prefix;
        std::string command;
        std::string params;
};