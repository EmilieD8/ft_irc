/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Binding.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: edrouot <edrouot@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/14 11:59:04 by mschaub           #+#    #+#             */
/*   Updated: 2023/11/14 12:55:25 by edrouot          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "socket.hpp"
#include "irc.hpp"

class Binding : public Socket {
    private:
        int binding;
        void ConnectToNetwork(int sock, struct sockaddr_in address);
        
    public:
        Binding(int domain, int service, int protocol, int port, u_long interface);
        int getBinding();
};