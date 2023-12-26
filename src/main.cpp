#include "../inc/irc.hpp"
#include <signal.h>

bool g_isExit = false;
Server* g_server = NULL;

void clear_all(Server *server) {
    std::cout << "enter here" << std::endl;
    std::cout << "size of clients: " << server->get_clients().size() << std::endl;
    for (std::vector<User*>::iterator it = server->get_clients().begin(); it != server->get_clients().end(); it++) {
        std::cout << (*it)->get_nick() << std::endl;
        delete *it;
    }
    server->get_clients().clear();
    for (std::vector<Channel*>::iterator it = server->get_channels().begin(); it != server->get_channels().end(); it++) {
        delete *it;
    }
    server->get_channels().clear();
    for (std::vector<pollfd>::iterator it = server->get_pollfds()->begin(); it != server->get_pollfds()->end(); it++) {
        close(it->fd);
    }
    server->get_pollfds()->clear();
    delete server->get_pollfds();
    //close(server.fd);
    // delete server;
    //exit(EXIT_SUCCESS);
}

void signal_handler(int sig) {
    if (sig == SIGINT)
    {
        std::cout << "end of program" << std::endl;
        if (g_server)
        {
            clear_all(g_server);
           // delete g_server;

        }
        exit(EXIT_SUCCESS);
    }
}

int main(int argc, char **argv)
{
    if (argc != 3) {
        std::cerr << "Usage: ./ircserv  <port> <password>" << std::endl;
        exit(EXIT_FAILURE);
    }
    int port = std::atoi(argv[1]);
    std::string password = argv[2];
    //check if port is an int and check if password exists
    signal(SIGINT, signal_handler);
    try
    {
        Server server(port, password);
        g_server = &server;
        server.launchServer();
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }
    return 0;
}

/*TODO :
 *                                                                          if wrong password, should not throw exception closing the whole program -->DONE
 *                                                                          command kick --> DONE
 *                                                                          command invite
 *                                                                          nickname -> check if already taken and add symbol --> DONE
 *                                                                          nickname + user -> check if valid --> DONE
 *                                                                          join channel -> topic sent (to be checked) --> DONE
 *                                                                          private messages ->first user does not send --> DONE
 *                                                                          mode -> all of them to be implemented inside the channels --> DONE
 *                                                                          Wrong nick in welcome message --> DONE
 *                                                                          Implementing password in channel --> DONE
 *                                                                          Message when joining channel with list of users --> DONE
 *                                                                          Loop for password at connection --> DOES NOT WORK, IDEA DEPRECATED
 *                                                                          "Cannot use this command in that context" --> DONE
 *                                                                          Message when new user is joining the channel ? --> DONE
 *                                                                          operator status -> add @ to the nickname --> DONE
 *                                                                          mode -> +o -o + parsing to be corrected --> DONE 
 *                                                                          private messages from user to user (different argv ?) --> DONE
 *                                                                          if nobody else in channel, channel is deleted --> DONE
 *                                                                          command quit --> DONE
 *                                                                          Lag time after quit --> DONE
 * 
 * 
 *                                                                          Change the parsing into a 2D array --> DONE
 * signals 
 *                                                                          atoi port --> DONE
 * ERR_NICKNAMEINUSE -- still assign same nickname 
 *                                                                          set the cout correctly / colors ==> DONE
 *                                                                          cleanup variables not used --> DONE
 *                                                                          Align the "this->_nick", "get_nick()" and "_nick" --> DONE
 * send proper error message (to be checked)
 * sometimes we receive MODE #channel b --> apparently it is a ban but for nothing
 * 
 * 
 * CHECK WITH MAX:
 * attributes correctly put in constructor and destructor
 * which function to private or public
 * change everything which is critical into exceptions
 * leaks, take care of the proper destructor
 * 
 * 
 * */

