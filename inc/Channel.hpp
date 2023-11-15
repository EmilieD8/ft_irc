#include "irc.hpp"

class Channel {
    public:
        Channel();
        ~Channel();
        Channel(Channel const & src);
        Channel & operator=(Channel const & src);
    private:
        std::string name;
        std::string topic;
};