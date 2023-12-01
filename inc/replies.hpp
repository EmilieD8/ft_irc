#pragma once
#include "irc.hpp"

# define ERR_PASSWDMISMATCH "464 : incorrect password\r\n"

# define NICK(old_nick, nick) (":" + old_nick + " NICK " + nick + "\r\n")

# define ERR_NEEDMOREPARAMS(command) (command + ":Not enough parameters")

# define RPL_WELCOME(nick, user, host) ("001 " + nick + " :Welcome to the Internet Relay Network " + nick + "!" + user + "@" + host + "\r\n")

# define PONG(server) ("PONG :" + server + "\r\n")

# define ERR_NOORIGIN(command) (command + ":No origin specified")