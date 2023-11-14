/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Binding.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: edrouot <edrouot@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/14 12:00:46 by mschaub           #+#    #+#             */
/*   Updated: 2023/11/14 12:55:16 by edrouot          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

//#include "../inc/Binding.hpp"
#include "../inc/irc.hpp"

Binding::Binding(int domain, int service, int protocol, int port, u_long interface) : Socket(domain, service, protocol, port, interface)
{
    ConnectToNetwork(getSock(), getAddress());
    testConnection(binding);
}
void Binding::ConnectToNetwork(int sock, struct sockaddr_in address)
{
    binding = bind(sock, (struct sockaddr *)&address, sizeof(address));
    return ;
}

int Binding::getBinding()
{
    return binding;
}