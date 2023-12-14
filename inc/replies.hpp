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

# define ERR_NOTONCHANNEL(channel) ("442 " + channel + " :You're not on that channel\r\n")

# define RPL_TOPIC(nick, user, host, channel, topic) ("332 " + nick + "!" + user + "@" + host + " " + channel + " :" + topic + "\r\n")

# define ERR_CHANOPRIVSNEEDED(channel) ("482 " + channel + " :You're not channel operator\r\n")

# define RPL_NOTOPIC(nick, user, host, channel) ("331 " + nick + "!" + user + "@" + host + " " + channel + " :No topic is set\r\n")

# define ERR_CHANNELISFULL(nick, channel) ("471 " + nick + " " + channel + " :Cannot join channel (+l)\r\n")

# define MODE_CHANNELMSGWITHPARAM(channel, mode, param) (":localhost MODE #" + channel + " " + mode + " " + param + "\r\n")

# define PRIVMSG(nick, user, host, channel, msg) (":" + nick + "!" + user + "@" + host + " PRIVMSG " + channel + " :" + msg + "\r\n")