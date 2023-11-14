/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   irc.hpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: edrouot <edrouot@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/10/23 10:58:19 by mschaub           #+#    #+#             */
/*   Updated: 2023/11/14 11:33:21 by edrouot          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <iostream>
#include "Request.hpp"
#include "Response.hpp"
#include <sys/socket.h>
#include <netinet/in.h>
#include <cstdlib>
#include <unistd.h>
#include <cerrno>

#include "Binding.hpp"
#include "socket.hpp"
#include "ListeningSocket.hpp"

