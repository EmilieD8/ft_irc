#include "irc.hpp"

class User {
    public:
        User();
        ~User();
        User(User const & src);
        User & operator=(User const & src);
    private:
        bool isOperator;
};
