#pragma once

# include "Binding.hpp"

class Listening : public Binding
{
    private:
        int backlog;
        int listening;
    public:
        Listening(int odmain, int service, int protocol, int port, u_long interface, int backlog);
        void startListening();
};