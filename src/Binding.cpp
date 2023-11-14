/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Binding.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mschaub <mschaub@student.42berlin.de>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/14 12:00:46 by mschaub           #+#    #+#             */
/*   Updated: 2023/11/14 12:00:46 by mschaub          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

//#include "../inc/Binding.hpp"
#include "../inc/irc.hpp"

Binding::Binding(int domain, int service, int protocol, int port, u_long interface) : Socket(domain, service, protocol, port, interface)
{
    setConnection(ConnectToNetwork(getSock(), getAddress()));
    testConnection(getConnection());
}
int Binding::ConnectToNetwork(int sock, struct sockaddr_in address)
{
    return bind(sock, (struct sockaddr *)&address, sizeof(address));
}