#include "../inc/irc.hpp"
#include <signal.h>

void signal_handler(int sig) {
    if (sig == SIGINT)
        std::cout << "end of program" << std::endl;
    exit(EXIT_SUCCESS);
}

int main(int argc, char **argv)
{
    if (argc != 3) {
        std::cerr << "Usage: ./ircserv  <port> <password>" << std::endl;
        exit(EXIT_FAILURE);
    }
    signal(SIGINT, signal_handler);
    try
    {
        int port = std::stoi(argv[1]);
        std::string password = argv[2];
        Server server(port, password);
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
 * 
 *                                                                          Message when joining channel with list of users --> DONE
 *                                                                          Loop for password at connection --> DOES NOT WORK, IDEA DEPRECATED
 *                                                                          "Cannot use this command in that context" --> DONE
 *                                                                          Message when new user is joining the channel ? --> DONE
 *                                                                          operator status -> add @ to the nickname --> DONE
 * mode -> +o -o + parsing to be corrected
 *                                                                          private messages from user to user (different argv ?) --> DONE
 *                                                                          if nobody else in channel, channel is deleted --> DONE
 *                                                                          command quit --> DONE
 *                                                                          Lag time after quit --> DONE
 * 
 * 
 *                                                                          Change the parsing into a 2D array --> DONE
 * signals
 *                                                                          atoi port --> DONE
 * ERR_NICKNAMEINUSE
 * set the cout correctly / colors
 *                                                                          cleanup variables not used --> DONE
 *                                                                          Align the "this->_nick", "get_nick()" and "_nick" --> DONE
 * send proper error message (to be checked)
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

