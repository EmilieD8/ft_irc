/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: edrouot <edrouot@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/10/22 13:58:25 by mschaub           #+#    #+#             */
/*   Updated: 2023/11/13 14:10:36 by edrouot          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../inc/webserv.hpp"

int main(int argc, char **argv) 
{
    // if (argc != 2) 
    // {
    //     // default configuration
    //     std::cerr << "Using default configuration." << std::endl;
    // }
    // else 
    // {
    //     // custom configuration
    //     std::cerr << "Using custom configuration." << std::endl;
    // }
    // argv[0] = NULL;

    //test from https://ncona.com/2019/04/building-a-simple-server-with-cpp/
    // int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    // if (sockfd == -1)
    // {
    //     std::cout << "Error creating the socket. Error: " << errno << std::endl;
    //     exit(EXIT_FAILURE);
    // }
    // sockaddr_in sockaddr;
    // sockaddr.sin_family = AF_INET;
    // sockaddr.sin_addr.s_addr = INADDR_ANY;
    // sockaddr.sin_port = htons(9999);
    
    // if (bind(sockfd, (struct sockaddr*)&sockaddr, sizeof(sockaddr)) < 0)
    // {
    //     std::cout << "Failed to bind to port 8888. Error: " << errno << std::endl;
    //     exit(EXIT_FAILURE);
    // }

    // if (listen(sockfd, 10) < 0)
    // {
    //     std::cout << "Failed to listen on socket. Error: " << errno << std::endl;
    //     exit(EXIT_FAILURE);
    // }

    // auto addrlen = sizeof(sockaddr);
    // int connection = accept(sockfd, (struct sockaddr*)&sockaddr, (socklen_t*)&addrlen);
    // if (connection < 0)
    // {
    //     std::cout << "Failed to grab connection. Error: " << errno << std::endl;
    //     exit(EXIT_FAILURE);
    // }

    // char buffer[100];
    // auto bytesRead = read(connection, buffer, 100);
    // if (bytesRead > 0)
    //     std::cout << "The message was "<< buffer;
    
    // std::string response = "Hey !\n";
    // send(connection, response.c_str(), response.size(), 0);

    // close(connection);
    // close(sockfd);
}