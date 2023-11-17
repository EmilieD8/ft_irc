#include "../inc/irc.hpp"

Server::Server(int port, std::string password) {
    _port = port;
    _name = "Lit Server";
    _password = password;
}

Server::~Server() {}

Server::Server(Server const & src) {
    *this = src;
}

Server & Server::operator=(Server const & src) {
    if (this != &src) {
        _port = src._port;
        _name = src._name;
        _password = src._password;
    }
    return *this;
}

void Server::launchServer() {
    int client;
    int server;
    int bufferSize = 1024;
    bool isExit = false;
    char buffer[bufferSize];
    socklen_t size;
    struct sockaddr_in server_addr;
    int rc;

    std::cout << "Launching " << _name << std::endl;
    std::cout << "Listening on port " << _port << std::endl;

    client = socket(AF_INET, SOCK_STREAM, 0);
    if (client < 0) {
        std::cout << "Error establishing the socket..." << std::endl;
        exit(EXIT_FAILURE);
    }

    std::cout << "Socket is created" << std::endl;
    int setting;
    /* set the sockets to be non blocking. Since the first one is created non blocking,
    all the others received after via poll will also be non blocking. */
    setting = fcntl(client, F_GETFL, 0);
    fcntl(client, F_SETFL, setting | O_NONBLOCK); 


    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htons(INADDR_ANY);
    server_addr.sin_port = htons(_port);
    // do we want to check that the port is superior to 1500 to avoid anything not working ?
    if (bind(client, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        std::cout << "Error binding" << std::endl;
        exit(EXIT_FAILURE);
    }

    std::cout << "Looking for clients " << std::endl;
    listen(client, 1); 
    // throw an error for issue with listening
    int clientCount = 1;

    _pollFds = new std::vector<pollfd>(maxClients + 1);

    std::vector<pollfd> &connectionFds = *_pollFds;
    connectionFds[0].fd = client;
    connectionFds[0].events = POLLIN; // means "there is data to read"

    int timeout = 3 * 60 * 1000; 
    // 3 min in millisecond, it's going to be the time allowed for the conenction to connect in poll
    int nbConnection = 1;
    do 
    {
        std::cout << "Waiting on polling...\n";
        rc = poll(connectionFds, nbConnection, timeout);
        if (rc < 0)
        {
            std::cerr << "Poll failed" << std::endl;
            break;
        }
        if (rc == 0)
        {
            std::cerr << "Poll timed out" << std::endl;
            break;
        }

        for (i = 0; i < (int const)nbConnection; i++)
        {
            if (connectionFds[i].revents == 0)
                continue;
            if (connectionFds[i].revents != POLLIN)
            {
                std::cout << "Error revent #"<< connectionFds[i].revents << std::endl;
                isExit = true;
                break;
            }
            if (connectionFds[i].fd = client)
            {
                std::cout << "Found readable socket" << std::endl;
                do
                {
                    server =  accept(client, (struct sockaddr *)&server_addr, &size);
                    if (server < 0)
                    {
                        std::cout << "Error accepting" << std::endl;
                        break;
                    }
                    std::cout << "New incoming connection #" << server << std::endl;
                    connectionFds[nbConnection].fd = server;
                    connectionFds[nbConnection].events = POLLIN;
                    nbConnection += 1;
                } while (server != -1);
            }
            else
            {
//                 r******************************/
//           rc = send(fds[i].fd, buffer, len, 0);
//           if (rc < 0)
//           {
//             perror("  send() failed");
//             close_conn = TRUE;c = recv(fds[i].fd, buffer, sizeof(buffer), 0);
//           if (rc < 0)
//           {
//             if (errno != EWOULDBLOCK)
//             {
//               perror("  recv() failed");
//               close_conn = TRUE;
//             }
//             break;
//           }

//           /*****************************************************/
//           /* Check to see if the connection has been           */
//           /* closed by the client                              */
//           /*****************************************************/
//           if (rc == 0)
//           {
//             printf("  Connection cl
//             break;
//           }

//         } while(TRUE);

//         /*******************************************************/
//         /* If the close_conn flag was turned on, we need       */
//         /* to clean up this active connection. This            */
//         /* clean up process includes removing the              */
//         /* descriptor.                                         */
//         /*******************************************************/
//         if (close_conn)
//         {
//           close(fds[i].fd);
//           fds[i].fd = -1;
//           compress_array = TRUE;
//         }


//       }  /* End of existing connection is readable             */
//     } /* End of loop through pollable descriptors              */

//     /***********************************************************/
//     /* If the compress_array flag was turned on, we need       */
//     /* to squeeze together the array and decrement the number  */
//     /* of file descriptors. We do not need to move back the    */
//     /* events and revents fields because the events will always*/
//     /* be POLLIN in this case, and revents is output.          */
//     /***********************************************************/
//     if (compress_array)
//     {
//       compress_array = FALSE;
//       for (i = 0; i < nfds; i++)
//       {
//         if (fds[i].fd == -1)
//         {
//           for(j = i; j < nfds-1; j++)
//           {
//             fds[j].fd = fds[j+1].fd;
//           }
//           i--;
//           nfds--;
//         }
//       }
//     }

//   } while (end_server == FALSE); /* End of serving running.    */

//   /*************************************************************/
//   /* Clean up all of the sockets that are open                 */
//   /*************************************************************/
//   for (i = 0; i < nfds; i++)
//   {
//     if(fds[i].fd >= 0)
//       close(fds[i].fd);
//   }
//             }


//         }
//     }
          }