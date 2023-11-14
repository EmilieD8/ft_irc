/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ListeningSocket.hpp                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mschaub <mschaub@student.42berlin.de>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/14 11:55:10 by mschaub           #+#    #+#             */
/*   Updated: 2023/11/14 12:00:22 by mschaub          ###   ########.fr       */
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
};