/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Binding.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: edrouot <edrouot@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/14 12:00:46 by mschaub           #+#    #+#             */
/*   Updated: 2023/11/14 14:36:27 by edrouot          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

//#include "../inc/Binding.hpp"
#include "../inc/irc.hpp"

Binding::Binding(int domain, int service, int protocol, int port, u_long interface) : Socket(domain, service, protocol, port, interface) {
    std::cout << "Here" << std::endl;
    ConnectToNetwork(getSock(), getAddress());
    testConnection(binding);
    std::cout << "Failure1" << std::endl;
}

void Binding::ConnectToNetwork(int sock, struct sockaddr_in address) {
    binding = bind(sock, (struct sockaddr *)&address, sizeof(address));
    return ;
}

int Binding::getBinding() {
    return binding;
}