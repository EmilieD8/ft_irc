/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   irc.hpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: edrouot <edrouot@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/10/23 10:58:19 by mschaub           #+#    #+#             */
/*   Updated: 2023/11/17 11:41:01 by edrouot          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <iostream>

#include <sys/socket.h>
#include <netinet/in.h>
#include <cstdlib>
#include <unistd.h>
#include <cerrno>
#include <vector>
#include <poll.h>

#include <iostream>
#include <string.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <netdb.h>
#include <fcntl.h>
#include <sstream>

#include "Server.hpp"
#include "User.hpp"
#include "replies.hpp"
#include "Channel.hpp"

#define BUFFER_SIZE 512
