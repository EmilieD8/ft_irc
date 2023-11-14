#pragma once

#include "Binding.hpp"
#include "irc.hpp"

class Listening : public Binding
{
    private:
        int backlog;
        int listening;
    public:
        Listening(int domain, int service, int protocol, int port, u_long interface, int backlog);
        void startListening();
};