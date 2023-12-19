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
 * change everything which is critical into exceptions
 *                                                                          command kick --> DONE
 *                                                                          command invite
 *                                      command quit
 *                                                                          nickname -> check if already taken and add symbol --> DONE
 *                                                                          nickname + user -> check if valid --> DONE
 *                                                                          join channel -> topic sent (to be checked) --> DONE
 *                                                                          private messages ->first user does not send --> DONE
 * private messages from user to user (different argv ? )
 * leaks, take care of the proper destructor
 * operator status -> add @ to the nickname
 * mode -> +o -o + parsing to be corrected
 *                                                                          mode -> all of them to be implemented inside the channels --> DONE
 * send proper error message (to be checked)
 * cleanup variables not used
 * welcome message
 * who is ?
 * set the cout correctly
 * "Cannot use this command in that context"
 * 
 * */