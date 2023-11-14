/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ListeningSocket.hpp                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: edrouot <edrouot@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/14 11:55:10 by mschaub           #+#    #+#             */
/*   Updated: 2023/11/14 14:32:35 by edrouot          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "Binding.hpp"
#include "irc.hpp"

class Listening : public Binding
{
    private:
        int backlog;
        int listening;
    public:
        Listening(int domain, int service, int protocol, int port, u_long interface, int backlog);
        void startListening();
        int getListening();
        int getBacklog();
};