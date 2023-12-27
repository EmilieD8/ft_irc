#include "../inc/irc.hpp"

User::User() {
    _fd = -1;
    _nick = "\0";
    _name = "\0";
    _realName = "\0";
    _hostName = "\0";
    _serverName = "\0";
    _passwordChecked = false;
    _channel_rn = NULL;
    _isInAChannel = false;
    std::string colors[] = {YELLOW, BLUE, MAGENTA, CYAN, BRIGHT_BLUE, BRIGHT_GREEN, BRIGHT_CYAN, BRIGHT_MAGENTA, BRIGHT_YELLOW};
    _color = colors[_fd % 9];
}

User::User(int fd) : _fd(fd)  {
    _nick = "\0";
    _name = "\0";
    _realName = "\0";
    _hostName = "\0";
    _serverName = "\0";
    _passwordChecked = false;
    _channel_rn = NULL;
    _isInAChannel = false;
    std::string colors[] = {YELLOW, BLUE, MAGENTA, CYAN, BRIGHT_BLUE, BRIGHT_GREEN, BRIGHT_CYAN, BRIGHT_MAGENTA, BRIGHT_YELLOW};
    _color = colors[_fd % 9];
}

User::~User() {
    return;
}

User::User(User const &src) {
    *this = src;
}

User &User::operator=(User const &src) {
    if (this != &src) {
        _fd = src._fd;
        _nick = src._nick;
        _name = src._name;
        _realName = src._realName;
        _hostName = src._hostName;
        _serverName = src._serverName;
        _passwordChecked = src._passwordChecked;
        _channel_rn = src._channel_rn;
        _isInAChannel = src._isInAChannel;
    }
    return *this;
}

int User::get_fd() const {
    return _fd;
}

std::string User::get_nick() const {
    return _nick;
}
std::string User::get_name() const {
    return _name;
}

std::string User::get_host() const {
    return _hostName;
}

void User::set_channel_atm(Channel& channel) {
    _channel_rn = &channel;
    _isInAChannel = true;
}

void User::setOperatorStatus(Channel &channel, bool isOperator) {
    _operatorStatusMap[&channel] = isOperator;
}

bool User::get_operatorStatus(Channel* channel) const {
    std::map<Channel*, bool>::const_iterator it = _operatorStatusMap.find(channel);
    if (it->second == true)
        return true;
    else
        return false;
}

void User::setInviteStatus(Channel &channel, bool isInvited) {
    _isInvitedToChannel[&channel] = isInvited;
}

bool User::get_InviteStatus(Channel* channel) const {
    std::map<Channel*, bool>::const_iterator it = _isInvitedToChannel.find(channel);
    if (it == _isInvitedToChannel.end())
        return false;
    if (it->second == true)
        return true;
    else
        return false;
}

Channel *User::get_channel_atm() const {
    return _channel_rn;
}
void User::set_nick(std::string nick) {
    _nick = nick;
}

void User::set_name(std::string name) {
    _name = name;
}

void	User::send_to(std::string text) const {
	send(_fd, text.c_str(), text.length(), 0);
	std::cout << GREEN << _fd - 3 << " > " << text;
}

void User::splitMessage(Server &server, std::string buf) {
    std::stringstream ss(buf);
    std::string word;
    int count;
    _message._paramsSplit.clear();
    _message._params.clear();
    if (buf[0] == ':')
        count = 0;
    else
        count = 1;
    while (ss >> word) {
        if (count == 0)
            _message._prefix = word;
        else if (count == 1)
            _message._command = word;
        else if (count == 2)
        {
            _message._paramsSplit.push_back(word);
            _message._params = word;
        }
        else
        {
            _message._paramsSplit.push_back(word);
            _message._params += " " + word;
        }
        count++;
    }
    parseMessage(server);
}

void User::parseMessage(Server &server) {
    std::string type[] = {"PASS", "NICK", "USER", "JOIN", "KICK", "INVITE", "CAP", "PING", "MODE", "TOPIC", "PRIVMSG", "PART", "QUIT", "/INVALID"};
    int count = 0;
    size_t arraySize = sizeof(type) / sizeof(type[0]);
    for (int i = 0; i < (int)arraySize; i++){
        if (_message._command.compare(type[i]) != 0)
            count++;
        else
            break;
    }
    switch (count) {
        case 0:
            command_pass(server);
            break;
        case 1:
            command_nick(server);
            break;
        case 2:
            command_user();
            break;
        case 3:
            command_join(server);
            break;
        case 4:
            command_kick();
            break;
        case 5:
            command_invite(server);
            break;
        case 6:
            //TODO CAP
            break;
        case 7:
            command_ping();
            break;
        case 8:
            command_mode(server);
            break;
        case 9:
            command_topic(server);
            break;
        case 10:
            command_privmsg(server);
            break;
        case 11:
            command_part(server);
            break;
        case 12:
            command_quit(server);
            break;
        case 13:
            std::cout << "Error: invalid command" << std::endl;// check if correct. maybe to put the help ? 
            break;
    }
}

void User::command_pass(Server &server) {
    std::cout << _color << _fd - 3 << " < PASS " << _message._params << std::endl;
    if (_message._params.compare(server.get_password()) != 0) {
        send_to(ERR_PASSWDMISMATCH);
    }
    else
        _passwordChecked = true;
}

void User::command_nick(Server &server) {
    std::cout << _color << _fd - 3 << " < NICK " << _message._params << std::endl;
    if (_passwordChecked == false)
        return;
    std::string new_nick = _message._params;
    if (_nick.empty() && (_message._params.find(' ') != std::string::npos || _message._params.size() > 9))
        new_nick = new_nick.substr(0, 9);
    if (_message._params.find(' ') != std::string::npos || _message._params.size() > 9) {
        send_to(ERR_ERRONEUSNICKNAME(new_nick));
        return ;
    }
    std::string old = get_nick();
    if (new_nick.empty()) {
        send_to(ERR_NONICKNAMEGIVEN(_message._command));
        return ;
    }
    std::vector <User *> clients = server.get_clients();
    for (std::vector<User *>::iterator it = clients.begin(); it != clients.end(); it++) {
        if ((*it)->get_nick() == _message._params) {
            send_to(ERR_NICKNAMEISUSE(_message._params));
            return;
        }
    }
    if (!_nick.empty() && !old.empty()) {
        send_to(NICK(old, new_nick));
        for (std::vector<Channel *>::iterator it = server.get_channels().begin() ; it != server.get_channels().end();it++) {
            for (std::vector<User *>::iterator it2 = (*it)->get_users().begin(); it2 != (*it)->get_users().end(); it2++) {
                if ((*it2)->get_nick() == old) {
                    (*it)->send_to_all_macro(NICK(old, new_nick), false, this);
                }
            }
        }
        set_nick(new_nick);
        return;
    }
    set_nick(new_nick);
    if (_passwordChecked && !_name.empty())
        send_to(RPL_WELCOME(_nick, _name, _hostName));
    return ;
}

void User::command_user() {
    std::cout << _color << _fd - 3 << " < USER " << _message._params << std::endl;
    if (_message._paramsSplit.size() < 3 || _message._paramsSplit[3][0] != ':') {
       send_to(ERR_NEEDMOREPARAMS(_message._command));
        return ;
    }
    _name = _message._paramsSplit[0];
    _hostName = _message._paramsSplit[1];
    _serverName = _message._paramsSplit[2];
    for (int i = 4; i < (int)_message._paramsSplit.size(); i++) {
    _realName = _message._paramsSplit[3].substr(1);
        _realName += " " + _message._paramsSplit[i];
    }
    if (_name.size() > 9) {
        _name = _name.substr(0, 9);
    }
    if (!_nick.empty())
        send_to(RPL_WELCOME(_nick, _name, _hostName));
    return ;
}

void User::command_ping() {
    std::cout << _color << _fd - 3 << " < PING " << _message._params << std::endl;
    if (_message._params.empty()) {
        send_to(ERR_NOORIGIN(_message._command));
        return;
    }
    else
        send_to(PONG(_message._params));
}

void User::command_join(Server &server) {
    std::cout << _color << _fd - 3 << " < JOIN " << _message._params << std::endl;
    int i = 0;
    std::string password;
    std::string channelName = _message._paramsSplit[0];
    if (channelName[0] != '#') {
        send_to(ERR_NOSUCHCHANNEL(channelName));
        return;
    }
    if (_message._paramsSplit.size() == 2)
        password = _message._paramsSplit[1];
    for (std::vector<Channel *>::iterator it = server.get_channels().begin(); it != server.get_channels().end(); it++) {
        if ((*it)->get_name() == channelName) {
            if ((*it)->get_limitSet()) {
                if ((*it)->get_userSize() + 1 > (*it)->get_limit()) {
                    send_to(ERR_CHANNELISFULL(_nick, channelName));
                    return;
                }
            }
            if ((*it)->get_inviteOnly()) {
                if (get_InviteStatus(*it) == false) {
                    send_to(ERR_INVITEONLYCHAN(_nick, channelName));
                    return;
                }
            }
            if((*it)->get_keySet()) {
                if ((*it)->get_password() != password)
                {
                    send_to(ERR_BADCHANNELKEY(_nick, channelName));
                    return;
                }
            }
            (*it)->add_user(*this);
            set_channel_atm(**it);
            setOperatorStatus(**it, false);
            _channel_rn->send_to_all_macro(JOIN(_nick, _name, _hostName, (*it)->get_name()), true, this);   
            if (!_channel_rn->get_topic().empty())
                send_to(RPL_TOPIC(_nick, _name, _hostName, _channel_rn->get_name(),  _channel_rn->get_topic()));
            std::string usersInChannel;
            for (std::vector<User *>::iterator it = _channel_rn->get_users().begin(); it != _channel_rn->get_users().end(); it++) {
                if ((*it)->get_operatorStatus(_channel_rn))
                    usersInChannel += "@" +(*it)->get_nick() + " ";
                else
                    usersInChannel += (*it)->get_nick() + " ";
            }
            send_to(RPL_NAMREPLY(_nick, _name, _hostName, _channel_rn->get_name()) + usersInChannel + "\r\n");
            send_to(RPL_ENDOFNAMES(_nick, _name, _hostName, _channel_rn->get_name()));
            break;
        } 
        else
            i++;
    }
    if (i == (int)server.get_channels().size()) {
        Channel *channel = new Channel(channelName);
        channel->add_user(*this);
        set_channel_atm(*channel);
        setOperatorStatus(*channel, true);
        std::string usersInChannel = "@" + _nick;
        send_to(RPL_YOUREOPER(_nick));
        send_to(JOIN(_nick, _name, _hostName, channel->get_name()));
        send_to(RPL_NAMREPLY(_nick, _name, _hostName, _channel_rn->get_name()) + usersInChannel + "\r\n");
        send_to(RPL_ENDOFNAMES(_nick, _name, _hostName, _channel_rn->get_name()));
        server.get_channels().push_back(channel);
    }
}

void User::command_topic(Server &server) {
    std::cout << _color << _fd - 3 << " < TOPIC " << _message._params << std::endl;
    if (_isInAChannel)
    {
        if (_channel_rn->get_topicRestricted())
        {
            if (!get_operatorStatus(_channel_rn))
            {
                send_to(ERR_CHANOPRIVSNEEDED(_channel_rn->get_name()));
                return;
            }
        }
        std::string nameTopic;
        if (_message._paramsSplit[1][0] == ':')
            nameTopic = _message._paramsSplit[1].substr(1);
        else
            nameTopic = _message._paramsSplit[1];
        for (int i = 2; i < (int)_message._paramsSplit.size(); i++)
            nameTopic += " " + _message._paramsSplit[i];
        if (nameTopic.empty())
        {
            if (_channel_rn->get_topic().empty())
                send_to(RPL_NOTOPIC(_nick, _name, _hostName, _channel_rn->get_name()));
            else
                send_to(RPL_TOPIC(_nick, _name, _hostName, _channel_rn->get_name(), _channel_rn->get_topic()));
        }
        else
        {
            _channel_rn->set_topic(nameTopic);
            _channel_rn->send_to_all_macro(RPL_TOPIC(_nick, _name, _hostName, _channel_rn->get_name(), nameTopic), true, this);
        }
    }
    else
    {
        if (_message._paramsSplit.size() == 0) {
            send_to(ERR_NEEDMOREPARAMS(_message._command));
            return;
        }
        bool skipped = false;
        for (std::vector <Channel *>::iterator it = server.get_channels().begin(); it != server.get_channels().end(); it++) {
            if ((*it)->get_name() == _message._paramsSplit[0]) {
                for (std::vector <User *>::iterator it2 = (*it)->get_users().begin(); it2 != (*it)->get_users().end(); it2++) {
                    if ((*it2)->get_nick() == _nick) {
                        if (!get_operatorStatus((*it))) {
                            send_to(ERR_CHANOPRIVSNEEDED( _message._paramsSplit[0]));
                            return;
                        }    
                        else {
                            skipped = true;
                            break;
                        } 
                    }
                }
                if (!skipped) {
                    send_to(ERR_NOTONCHANNEL( _message._paramsSplit[0]));
                    return;
                }
                break;
            }
        }
        if (!skipped) {
            send_to(ERR_NOSUCHCHANNEL( _message._paramsSplit[0]));
            return;
        }
    }
}

void User::command_mode(Server &server) {
    std::cout << _color << _fd - 3 << " < MODE " << _message._params << std::endl;
    std::stringstream ss(_message._params);
    std::string word;
    std::string channel;
    std::string flags = "\0";
    std::vector<std::string> optionsArray;
    int count = 0;
    Channel *channel_ptr;
    while (ss >> word) {
        if (count == 0 && _isInAChannel == true && word != _channel_rn->get_name())
            flags = word;
        else
            channel = word;
        if (count == 1)
            flags += word;
        else if (count != 0)
            optionsArray.push_back(word);
        count++;
    }
    bool skipped = false;
    if (!_isInAChannel) {
        for (std::vector <Channel *>::iterator it = server.get_channels().begin(); it != server.get_channels().end(); it++) {
            if ((*it)->get_name() == channel) {
                channel_ptr = (*it);
                for (std::vector <User *>::iterator it2 = (*it)->get_users().begin(); it2 != (*it)->get_users().end(); it2++) {
                    if ((*it2)->get_nick() == _nick) {
                        if (!get_operatorStatus((*it))) {
                            send_to(ERR_CHANOPRIVSNEEDED(channel));
                            return;
                        }    
                        else {
                            skipped = true;
                            break;
                        } 
                    }
                }
                if (!skipped) {
                    send_to(ERR_NOTONCHANNEL(channel));
                    return;
                }
                break;
            }
        }
        if (!skipped) {
            std::string test = _nick + " +i";
            if (_message._params == test)
                return;
            send_to(ERR_NOSUCHCHANNEL(channel));
            return;
        }
    }
    if (flags.empty())
    {
        std::string modes = "+";
        for (std::vector <Channel *>::iterator it = server.get_channels().begin(); it != server.get_channels().end(); it++) {
            if ((*it)->get_name() == channel)
            {
                channel_ptr = (*it);
                break;
            }
        }
        if (channel_ptr->get_inviteOnly())
            modes += "i";
        if (channel_ptr->get_topicRestricted())
            modes += "t";
        if (channel_ptr->get_limitSet())
            modes += "l";
        if (channel_ptr->get_keySet())  
            modes += "k";
        if (modes == "+")
            modes = "\0";
        send_to(RPL_CHANNELMODEIS(_serverName, _nick, channel, modes));
        return;
    } 
    if (flags != "\0" && _isInAChannel && get_operatorStatus(_channel_rn) == false) {
        send_to(ERR_CHANOPRIVSNEEDED(_channel_rn->get_name()));
        return;
    }
    s_flag *parsed;
    if (flags[0] == '+' || flags[0] == '-') {
        parsed = parserOption(flags);
        if (checkParsing(parsed, optionsArray))
            interpretMode(parsed, optionsArray, (*_channel_rn));
        s_flag *currentFlag = parsed;
        while (currentFlag != NULL){
            s_flag *nextFlag = currentFlag->next;
            delete currentFlag;
            currentFlag = nextFlag; 
        }
    }
    return;
}

bool User::checkParsing(s_flag *parsed, std::vector<std::string> options)
{
    int i = 0;
    while (parsed != NULL)
    {
        if (parsed->flag == 'i' || parsed->flag == 't')
        {
            if (parsed->sign != 1 && parsed->sign != 2)
                parsed->isValid = false;
        }
        else if ((parsed->flag == 'k' || parsed->flag == 'l' || parsed->flag == 'o') && (parsed->sign == 1 || parsed->sign == 2))
        {
            if (parsed->sign == 1)
            {
                if (options.empty() || i >= (int)options.size())
                    return (false);
                else
                    parsed->option = options[i];
                i++;
            }
        }
        else
            return (false);
        parsed = parsed->next;
    }
    return (true);
}
void User::interpretMode(s_flag *parsed, std::vector<std::string> options, Channel &channel)
{
    int i = 0;
    std::string toSendFlagsPos;
    std::string toSendFlagsNeg;
    std::string toSendOptions;
    while(parsed != NULL)
    {
        if (parsed->flag == 't' && parsed->sign == 1)
        {
            if (!channel.get_topicRestricted())
            {
                channel.set_topicRestricted(true);
                toSendFlagsPos += "t";
            }
        }
        else if (parsed->flag == 't' && parsed->sign == 2)
        {
            if (channel.get_topicRestricted())
            {
                channel.set_topicRestricted(false);
                toSendFlagsNeg += "t";
            }
        }
        else if (parsed->flag == 'i' && parsed->sign == 1)
        {
            if (!channel.get_inviteOnly())
            {
                channel.set_inviteOnly(true);
                toSendFlagsPos += "i";
            }
        }
        else if (parsed->flag == 'i' && parsed->sign == 2)
        {
            if (channel.get_inviteOnly())
            {
                channel.set_inviteOnly(false);
                toSendFlagsNeg += "i";
            }
        }
        else if (parsed->flag == 'l' && parsed->sign == 1)
        {
            try
            {
                char *endptr;
                int limit = (int)std::strtol(options[i].c_str(),  &endptr, 10);
                if (limit < 0 || *endptr != '\0')
                    throw std::exception();
                channel.set_limitSet(true);
                channel.set_limit(limit);
                std::string flags = "+l " + options[i];
                toSendFlagsPos += "l";
                toSendOptions += options[i];
            }
            catch(const std::exception& e)
            {
                std::cerr << e.what() << '\n';
            }
            i++;
        }
        else if (parsed->flag == 'l' && parsed->sign == 2)
        {
            if (channel.get_limitSet())
            {
                channel.set_limitSet(false);
                toSendFlagsNeg += "l";
            }
        }
        else if (parsed->flag == 'k' && parsed->sign == 1)
        {
           
            channel.set_keySet(true);
            channel.set_password(options[i]);
            std::string flags = "+k " + options[i];
            toSendFlagsPos += "k";
            toSendOptions += options[i] + " ";
            i++;
        }
        else if (parsed->flag == 'k' && parsed->sign == 2)
        {
            if (channel.get_keySet())
            {
                channel.set_keySet(false);
                toSendFlagsNeg += "k";
            }
            i++;
        }
        else if (parsed->flag == 'o' && parsed->sign == 1)
        {
            std::string nickname = options[i];
            std::string flags = "+o " + options[i];
            bool changed_plus = false;
            for (std::vector<User *>::iterator it = channel.get_users().begin(); it != channel.get_users().end(); it++)
            {
                if ((*it)->get_nick() == nickname)
                {
                    (*it)->setOperatorStatus(*_channel_rn, true);
                    send_to(RPL_YOUREOPER(_nick));
                    changed_plus = true;
                    toSendFlagsPos += "o";
                    toSendOptions += options[i] + " ";
                    break;
                }
            }
            if (!changed_plus) {
                send_to(ERR_USERNOTINCHANNEL(nickname, channel.get_name()));
                continue;
            }
            i++;
        }
        else if (parsed->flag == 'o' && parsed->sign == 2)
        {
            std::string nickname = options[i];
            std::string flags = "-o " + options[i];
            bool changed_minus = false;
            for (std::vector<User *>::iterator it = channel.get_users().begin(); it != channel.get_users().end(); it++)
            {
                if ((*it)->get_nick() == nickname)
                {
                    (*it)->setOperatorStatus(*_channel_rn, false);
                    changed_minus = true;
                    toSendFlagsNeg += "o";
                    toSendOptions += options[i] + " ";
                    break;
                }
            }
            if (!changed_minus) {
                send_to(ERR_USERNOTINCHANNEL(nickname, channel.get_name()));
                continue;
            }
            i++;
        }
        else
            send_to(ERR_UNKNOWNMODE(_serverName, _nick, parsed->flag).c_str());
        parsed = parsed->next;
    }
    if (toSendFlagsPos.size() > 0)
        toSendFlagsPos = "+" + toSendFlagsPos;
    if (toSendFlagsNeg.size() > 0)
        toSendFlagsNeg = "-" + toSendFlagsNeg;
    if (toSendOptions.size() > 0)
        toSendOptions = " " + toSendOptions;
    std::string finalString = toSendFlagsPos + toSendFlagsNeg + toSendOptions;
    if (finalString.size() > 0)
        channel.send_to_all_macro(RPL_CHANNELMODEIS(_serverName, _nick, channel.get_name(), finalString), true, this);
    return;
}

s_flag *User::parserOption(std::string flags)
{
    int i = 0;
    s_flag* head = NULL;
    s_flag *currentFlag = NULL;
    int parsedSign = 0;
    while (flags[i] != '\0')
    {
        if (flags[i] == '+')
            parsedSign = 1;
        else if (flags[i] == '-')
            parsedSign = 2;
        else
        {
            s_flag *newFlag = new s_flag;
            if (head == NULL)
            {
                head = newFlag;
                head->prev = NULL;
                currentFlag = newFlag;
                currentFlag->prev = NULL;
            }
            else
            {
                currentFlag->next = newFlag;
                newFlag->prev = currentFlag;
                currentFlag = newFlag;
            }
            newFlag->flag = flags[i];
            if (parsedSign != 0)
                updateStruct(newFlag, parsedSign, true);
            else if (currentFlag != NULL && currentFlag->prev != NULL)
                updateStruct(newFlag, currentFlag->prev->sign, true);
            else
                updateStruct(newFlag, 0, false);
            parsedSign = 0;
        }
        i++; 
    }
    return (head);
}

s_flag *User::updateStruct(s_flag *newFlag, int sign, bool isValid)
{
    newFlag->sign = sign;
    newFlag->isValid = isValid;
    newFlag->next = NULL;
    return (newFlag);
}

void User::command_privmsg(Server &server) {
    std::cout << _color << _fd - 3 << " < MSG " << _message._params << std::endl;
    std::string user = "\0";
    std::string msg;
    std::cout << "is in a channel : " << _isInAChannel << std::endl;
    if (!_isInAChannel || (_isInAChannel && _message._paramsSplit[0] != _channel_rn->get_name()))
        user = _message._paramsSplit[0];
    for (int i = 1; i < (int)_message._paramsSplit.size(); i++)
        msg += _message._paramsSplit[i] + " ";
    if (msg[0] == ':')
        msg = msg.substr(1, msg.size() - 2);
    if (user == "\0")
        _channel_rn->send_to_all_private(msg, this, _nick);
    else
    {
        for (std::vector <User *>::iterator it2 = server.get_clients().begin(); it2 != server.get_clients().end(); it2++)
            if ((*it2)->get_nick() == user)
                (*it2)->send_to(PRIVMSG(_nick, _name, _hostName, user, msg));
    }
}

void User::command_part(Server &server) {
    std::cout << _color << _fd - 3 << " < PART " << _message._params << std::endl;
    std::string msg;
    for (int i = 1; i < (int)_message._paramsSplit.size(); i++)
        msg += _message._paramsSplit[i] + " ";
    if (!msg.empty())
        msg = msg.substr(1, msg.size() - 2);
    if (!_channel_rn) {
        send_to(ERR_NEEDMOREPARAMS(_message._command));
        return;
    }
    if (msg.empty())
        _channel_rn->send_to_all_macro(PART(_nick, _name, _hostName, _channel_rn->get_name()), true, this);
    else
        _channel_rn->send_to_all_macro(PART_REASON(_nick, _name, _hostName, _channel_rn->get_name(), msg), true, this);
    _channel_rn->remove_user(*this);
    _operatorStatusMap.erase(_channel_rn);
    _isInvitedToChannel.erase(_channel_rn);
    if (_channel_rn->get_userSize() == 0) {
        std::vector<Channel *>::iterator it = std::find(server.get_channels().begin(), server.get_channels().end(), _channel_rn);
        if (it != server.get_channels().end()) {
            server.get_channels().erase(it);
        }
        delete _channel_rn;
    }
    if (_operatorStatusMap.empty()){
        _channel_rn = NULL;
        _isInAChannel = false;
    }
    else {
        for (std::map<Channel *, bool>::iterator it = _operatorStatusMap.begin(); it != _operatorStatusMap.end(); ++it)
            _channel_rn = it->first;
        _isInAChannel = true;
    }
}

void User::command_kick() {
    std::cout << _color << _fd - 3 << " < KICK " << _message._params << std::endl;
    if (!get_operatorStatus(_channel_rn)) {
        send_to(ERR_CHANOPRIVSNEEDED(_channel_rn->get_name()));
        return;
    }
    else {
        std::string reason;
        std::string userToBeKicked = _message._paramsSplit[1];
        for (int i = 2; i < (int)_message._paramsSplit.size(); i++)
            reason += _message._paramsSplit[i] + " ";
        if (!reason.empty())
            reason = reason.substr(1, reason.size() - 2);
        bool found = false;
        if (!userToBeKicked.empty()) {
            for (std::vector<User *>::iterator it = _channel_rn->get_users().begin(); it != _channel_rn->get_users().end(); it++) {
                if ((*it)->get_nick() == userToBeKicked) {
                    _channel_rn->send_to_all_macro(KICK(_nick, _name, _hostName, _channel_rn->get_name(), userToBeKicked, reason), true, this);
                    found = true;
                    _channel_rn->remove_user(**it);
                    (*it)->_operatorStatusMap.erase(_channel_rn);
                    (*it)->_isInvitedToChannel.erase(_channel_rn);
                    (*it)->_channel_rn = NULL;
                    (*it)->_isInAChannel = false;
                    std::cout << "is in a channel in kick: " << _isInAChannel << std::endl;
                    // if (_operatorStatusMap.empty()){
                    //         _channel_rn = NULL;
                    //         _isInAChannel = false;
                    // }
                    // else {
                    //     for (std::map<Channel *, bool>::iterator it = _operatorStatusMap.begin(); it != _operatorStatusMap.end(); ++it)
                    //         _channel_rn = it->first;
                    //     _isInAChannel = true;
                    // }
                    break;
                }
            }
            if (!found)
                send_to(ERR_USERNOTINCHANNEL(userToBeKicked, _channel_rn->get_name()));
        }
        else
            send_to(ERR_NEEDMOREPARAMS(_message._command));
    }
}

void User::command_invite(Server &server) {
    std::cout << _color << _fd - 3 << " < INVITE " << _message._params << std::endl;
    if (!_isInAChannel) {
        send_to(ERR_NOTONCHANNEL(_channel_rn->get_name()));
        return;
    }
    if (!get_operatorStatus(_channel_rn)) {
        send_to(ERR_CHANOPRIVSNEEDED(_channel_rn->get_name()));
        return;
    }
    std::string userToBeInvited = _message._paramsSplit[0];
    std::string channel = _message._paramsSplit[1];
    bool found = false;
    if (!userToBeInvited.empty()) {
        for (std::vector<User *>::iterator it = server.get_clients().begin(); it != server.get_clients().end(); it++) {
            if ((*it)->get_nick() == userToBeInvited) {
                for (std::vector<User *>::iterator it = _channel_rn->get_users().begin(); it != _channel_rn->get_users().end(); it++) {
                    if ((*it)->get_nick() == userToBeInvited) {
                        send_to(ERR_USERONCHANNEL(_nick, userToBeInvited, channel));
                        return;
            }
        }
                (*it)->send_to(INVITE(_nick, _name, _hostName, userToBeInvited, channel));
                found = true;
                (*it)->setInviteStatus(*_channel_rn, true);
                return;
            }
        }
        if (!found)
            send_to(ERR_NOSUCHNICK(userToBeInvited));
    }
    else
        send_to(ERR_NEEDMOREPARAMS(_message._command));
}

void User::command_quit(Server &server) {
    std::cout << _color << _fd - 3 << " < QUIT " << _message._params << std::endl;
    Channel *channel_ptr = NULL;
    User *User_ptr = NULL;
    for (std::vector<Channel *>::iterator it = server.get_channels().begin(); it != server.get_channels().end(); ++it) {
        for (std::vector<User *>::iterator it2 = (*it)->get_users().begin(); it2 != (*it)->get_users().end(); ++it2) {
            if ((*it2)->get_nick() == _nick) {
                if (_message._params.empty())
                    (*it)->send_to_all_macro(PART(_nick, _name, _hostName, (*it)->get_name()), false, this);
                else
                    (*it)->send_to_all_macro(PART_REASON(_nick, _name, _hostName, (*it)->get_name(), _message._params), false, this);
                User_ptr = (*it2);
                _operatorStatusMap.erase((*it));
                _isInvitedToChannel.erase((*it));
                if ((*it)->get_userSize() == 0) {
                    std::vector<Channel *>::iterator it = std::find(server.get_channels().begin(),
                                                                    server.get_channels().end(), _channel_rn);
                    if (it != server.get_channels().end()) {
                        channel_ptr = *it;
                    }
                    break;
                }
            }
        }
        (*it)->remove_user(*User_ptr);
    }
    if (channel_ptr != NULL) {
        std::vector<Channel *>::iterator it = std::find(server.get_channels().begin(), server.get_channels().end(),
                                                        channel_ptr);
        if (it != server.get_channels().end()) {
            server.get_channels().erase(it);
            delete channel_ptr;
        }
    }
    for (std::vector<User *>::iterator it = server.get_clients().begin(); it != server.get_clients().end(); it++) {
        if ((*it)->get_nick() == _nick) {
            for (std::vector<pollfd>::iterator it2 = server.get_pollfds()->begin();
                    it2 != server.get_pollfds()->end(); it2++) {
                if ((*it2).fd == _fd) {
                    server.get_pollfds()->erase(it2);
                }
            }
            close(_fd);
            server.get_clients().erase(it);
            server.decrease_num_clients(1);
            delete this;
            break;
        }
    }
}


/*irc interpretation : 
* /mode +t +l 10 +o mimi ==> send #CHANNEL +tlo 10 mimi
* /mode +t -l 10 +o mimi ==> send +t-l10+o mimi
* /mode +t -l 58 +k hello ==> send +t-l58+k hello
* /mode +t +l 58 +k hello ==> send +tlk 58 hello
* /mode +t +l 58 -k hello ==> send +tl-k 58 hello 
* /mode +t +l 58 -k hello +o ==> send +tl-k+o 58 hello


* /mode +t +l ==> send "MODE #test +tl" ==> SEGFAULT   and should not do anything
* /mode +t +l 10 ==> send "MODE #test +tl 10" - set +t and + l10
-t -l with the limit, does not work 
issue with receiving some of the modes
/mode +k 10 +l pass => password set but not limit
==> in mine -> set the password to 10 and exception is thrown out for stoi
/mode +k pass +l 10 -> send MODE #test +kl pass 10
if one element is missing, wrong
save the messages in a message and send them all at once ? 
*/


