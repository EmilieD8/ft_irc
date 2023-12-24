#pragma once
#include "irc.hpp"

# define ERR_PASSWDMISMATCH "464 :Password incorrect\r\n"

# define NICK(old_nick, nick) (":" + old_nick + " NICK " + nick + "\r\n")

# define ERR_NEEDMOREPARAMS(command) (command + ":Not enough parameters\r\n")

# define RPL_WELCOME(nick, user, host) ("001 " + nick + " :Welcome to the Internet Relay Network " + nick + "!" + user + "@" + host + "\r\n")

# define PONG(server) ("PONG :" + server + "\r\n")

# define ERR_NOORIGIN(command) (command + ":No origin specified\r\n")

# define JOIN(nick, user, host, channel) (":" + nick + "!" + user + "@" + host  + " JOIN " + channel + "\r\n")

# define ERR_NONICKNAMEGIVEN(command) (command + ":No nickname given\r\n")

# define ERR_NICKNAMEISUSE(nick) ("433 NICK :" + nick + "\r\n")

# define ERR_NOTONCHANNEL(channel) ("442 " + channel + " :You're not on that channel\r\n")

# define RPL_TOPIC(nick, user, host, channel, topic) ("332 " + nick + "!" + user + "@" + host + " " + channel + " :" + topic + "\r\n")

# define ERR_CHANOPRIVSNEEDED(channel) ("482 " + channel + " :You're not channel operator\r\n")

# define RPL_NOTOPIC(nick, user, host, channel) ("331 " + nick + "!" + user + "@" + host + " " + channel + " :No topic is set\r\n")

# define ERR_CHANNELISFULL(nick, channel) ("471 " + nick + " " + channel + " :Cannot join channel (+l)\r\n")

# define MODE_CHANNELMSGWITHPARAM(channel, mode, param) (":localhost MODE #" + channel + " " + mode + " " + param + "\r\n")

# define PRIVMSG(nick, user, host, channel, msg) (":" + nick + "!" + user + "@" + host + " PRIVMSG " + channel + " :" + msg + "\r\n")

# define PART(nick, user, host, chan) (":" + nick + "!" + user + "@" + host + " PART " + chan + "\r\n")

# define PART_REASON(nick, user, host, chan, reason) (":" + nick + "!" + user + "@" + host + " PART " + chan + " :" + reason + "\r\n")

# define ERR_USERNOTINCHANNEL(nick, chan) ("441 " + nick + " " + chan + ": They aren’t on that channel\r\n")

# define RPL_CHANNELMODEIS(server, nick, channel, mode) (":" + server + " 324 " + nick + " " + channel + " " + mode + "\r\n")

#define  ERR_UNKNOWNMODE(servername, nick, mode) (":" + servername + " 472 " + nick + " " + mode + " :is unknown mode char to me\r\n")

# define ERR_ERRONEUSNICKNAME(nick) ("432 " + nick + " :Erroneous nickname\r\n")

# define RPL_YOUREOPER(nick) ("381 " + nick + " :You are now an IRC operator\r\n")

# define KICK(nick, user, host, chan, kicked, reason) (":" + nick + "!" + user + "@" + host + " KICK " + chan + " " + kicked + ": " + reason + "\r\n")

# define INVITE(nick, user, host, nickinvite, channel)  (":" + nick + "!" + user + "@" + host + " INVITE " + nickinvite + " " + channel + "\r\n")

# define ERR_NOSUCHNICK(invitenick) ("401 " + invitenick + " :No such nick/channel\r\n")

# define ERR_USERONCHANNEL(nick, invitenick, chan) ("443 " + nick + " " + invitenick + " " + chan + ":is already on channel\r\n")

# define ERR_INVITEONLYCHAN(nick, chan) ("473 " + nick + " " + chan + " :Cannot join channel (+i)\r\n")

# define QUIT(nick, user, host) (nick + "!" + user + "@" + host  + " QUIT\r\n")

# define QUIT_REASON(nick, user, host, msg) (nick + "!" + user + "@" + host  + " QUIT :" + msg + "\r\n")

# define ERR_BADCHANNELKEY(nick, chan) ("475 " + nick + " " + chan + " :Cannot join channel (+k)\r\n")

# define RPL_NAMREPLY(nick, user, host, channel) ("353 " + nick + "!" + user + "@" + host + " = " + channel + " :")

# define RPL_ENDOFNAMES(nick, user, host, channel) ("366 " + nick + "!" + user + "@" + host + " " + channel + " :End of NAMES list\r\n")

# define ERR_NOSUCHCHANNEL(channel) ("403 " + channel + " :No such channel\r\n")