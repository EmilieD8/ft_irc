/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Binding.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mschaub <mschaub@student.42berlin.de>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/14 11:59:04 by mschaub           #+#    #+#             */
/*   Updated: 2023/11/14 11:59:04 by mschaub          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "socket.hpp"
#include "irc.hpp"

class Binding : public Socket
{
    public:
        Binding(int domain, int service, int protocol, int port, u_long interface);
        int ConnectToNetwork(int sock, struct sockaddr_in address);
};