#include "../inc/irc.hpp"

int main()
{
    int client, server;
    int portNum = 6667;
    bool isExit = false;
    int bufferSize = 1024;
    char buffer[bufferSize];

    struct sockaddr_in server_addr;
    socklen_t size;

    client = socket(AF_INET, SOCK_STREAM, 0);
    if (client < 0)
    {
        std::cout << "Error establishing the socket..." << std::endl;
        exit(EXIT_FAILURE);
    }
    std::cout << "Socket is created" << std::endl;

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htons(INADDR_ANY);
    server_addr.sin_port = htons(portNum);


    if (bind(client, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
    {
        std::cout << "Error binding" << std::endl;
        exit(EXIT_FAILURE);
    }

    std::cout << "Looking for clients " << std::endl;
    listen(client, 1);
    int clientCount = 1;
    size = sizeof(server_addr);
    server = accept(client, (struct sockaddr *)&server_addr, &size);
    if (server < 0)
        std::cout << "Error accepting" << std::endl;

    while (server > 0)
    {
        strcpy(buffer, "Server connected...\n");
        send(server, buffer, bufferSize, 0);
        std::cout << "Connected with the client #" << clientCount << " ok" << std::endl;
        std::cout << "Enter # to end the connection" << std::endl;
        std::cout << "client:";
        recv(server, buffer, bufferSize, 0);
        while (*buffer != '*')
        {
            std::cout << buffer << " ";
            if (*buffer == '#')
            {
                *buffer = '*';
                isExit = true;
            }
            recv(server, buffer, bufferSize, 0);
        }

        while (!isExit)
        {
            std::cout << "\nServer: ";
            while (*buffer != '*')
            {
                std::cin >> buffer;
                send(server, buffer, bufferSize, 0);
                if (*buffer == '#') {
                    send(server, buffer, bufferSize, 0);
                    *buffer = '*';
                    isExit = true;
                }
            } 
            while (*buffer != '*')
            {
                std::cout << "Client: ";
                recv(server, buffer, bufferSize, 0);
                std::cout << buffer << " ";
                if (*buffer == '#') 
                {
                    *buffer = '*';
                    isExit = true;
                }
            } 
        }
        std::cout << "\n\n=> Connection terminated with IP " << inet_ntoa(server_addr.sin_addr);
        close(server);
        std::cout << "\nGoodbye..." << std::endl;
        isExit = false;
        exit(1);
    }

    close(client);
    return 0;
}