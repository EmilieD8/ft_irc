/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: edrouot <edrouot@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/10/22 13:58:25 by mschaub           #+#    #+#             */
/*   Updated: 2023/11/14 14:35:58 by edrouot          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../inc/irc.hpp"

int main(int argc, char **argv) {
    std::cout << "Starting..." << std::endl;
    std::cout << "Binding socket..." << std::endl;
    Binding bindsocket(AF_INET, SOCK_STREAM, 0, 6667, INADDR_ANY);
    std::cout << "Listening Socket..." << std::endl;
    Listening listensocket(AF_INET, SOCK_STREAM, 0, 6667, INADDR_ANY, 10);
    std::cout << "SUCESS!" << std::endl;

    if (argc != 3) {
        std::cerr << "Usage: ./ircserv  <port> <password>" << std::endl;
        exit(EXIT_FAILURE);
    }
    try {
        int port = std::stoi(argv[1]);
        std::string password = argv[2];
        /* Launch server */
    }
    catch (std::exception &e) {
        std::cerr << "Error: " << e.what() << std::endl;
        exit(EXIT_FAILURE);
    }
}