#include "../inc/irc.hpp"

int main(int argc, char **argv)
{
    if (argc != 3) {
        std::cerr << "Usage: ./ircserv  <port> <password>" << std::endl;
        exit(EXIT_FAILURE);
    }
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
 * "Cannot use this command in that context"
 *                                                                          Message when new user is joining the channel ? --> DONE
 *                                                                          operator status -> add @ to the nickname --> DONE
 * mode -> +o -o + parsing to be corrected
 *                                                                          private messages from user to user (different argv ?) --> DONE
 *                                                                          if nobody else in channel, channel is deleted --> DONE
 *                                                                          command quit --> DONE
 *                                                                          Lag time after quit --> DONE
 * 
 * 
 * Change the parsing into a 2D array
 * signals
 * 
 * set the cout correctly / colors
 * change everything which is critical into exceptions
 * cleanup variables not used
 * Align the "this->_nick", "get_nick()" and "_nick"
 * send proper error message (to be checked)
 * leaks, take care of the proper destructor
 * Private vs public attributes to be organized
 * */

