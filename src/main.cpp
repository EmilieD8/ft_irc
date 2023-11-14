/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: edrouot <edrouot@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/10/22 13:58:25 by mschaub           #+#    #+#             */
/*   Updated: 2023/11/14 11:37:06 by edrouot          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../inc/irc.hpp"

int main(int argc, char **argv) 
{
    std::cout << "Starting..." << std::endl;
    std::cout << "Binding socket..." << std::endl;
    Binding bindsocket(AF_INET, SOCK_STREAM, 0, 8000, INADDR_ANY);
    std::cout << "Listening Socket..." << std::endl;
    Listening listensocket(AF_INET, SOCK_STREAM, 0, 8000, INADDR_ANY, 10);
    std::cout << "SUCESS !" << std::endl; 
}