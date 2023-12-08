#pragma once
#include "irc.hpp"

# define ERR_PASSWDMISMATCH "464 : incorrect password\r\n"

# define NICK(old_nick, nick) (":" + old_nick + " NICK " + nick + "\r\n")

# define ERR_NEEDMOREPARAMS(command) (command + ":Not enough parameters")

# define RPL_WELCOME(nick, user, host) ("001 " + nick + " :Welcome to the Internet Relay Network " + nick + "!" + user + "@" + host + "\r\n")

# define PONG(server) ("PONG :" + server + "\r\n")

# define ERR_NOORIGIN(command) (command + ":No origin specified")

# define JOIN(nick, user, host, channel) (":" + nick + "!" + user + "@" + host  + " JOIN " + channel + "\r\n")

# define ERR_NONICKNAMEGIVEN(command) (command + ":No nickname given")

# define ERR_NICKNAMEINUSE(command, nick) (command + nick + " :Nickname is already in use\r\n")

# define ERR_NOTONCHANNEL(chan) ("442 " + chan + " :You're not on that channel\r\n")

# define RPL_TOPIC(nick, user, host, chan, topic) ("332 " + nick + "!" + user + "@" + host + " " + chan + " :" + topic + "\r\n")