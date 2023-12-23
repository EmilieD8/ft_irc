#include "../inc/irc.hpp"

User::User() {
    _isInAChannel = false;
    _channel_rn = NULL;
    _nick = "\0";
    _name = "\0";
    _pw = "\0";
    _realName = "\0";
    _hostName = "\0";
    _serverName = "\0";
    _passwordChecked = false;
}

User::User(int fd, int id) : _fd(fd), id(id)  {
    _isInAChannel = false;
    _channel_rn = NULL;
    _nick = "\0";
    _name = "\0";
    _pw = "\0";
    _realName = "\0";
    _hostName = "\0";
    _serverName = "\0";
    _passwordChecked = false;
}

User::~User() {
   // delete this; ==> segfault
}

User::User(User const &src) {
    *this = src;
}

User &User::operator=(User const &src) {
    if (this != &src) {
        _fd = src._fd;
        id = src.id;
        // change to private attributes ++

    }
    return *this;
}

int User::get_fd() const {
    return _fd;
}

int User::get_id() const {
    return id;
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

std::string User::get_pw() const {
    return _pw;
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

void User::splitMessage(int fd, Server &server, std::string buf) {
    std::stringstream ss(buf);
    std::string word;
    int count;

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
            _message._params = word;
        else
            _message._params += " " + word;
        count++;
    }
    std::cout << _message._params << std::endl;
    parseMessage(server);
}

void User::parseMessage(Server &server) {
    std::string type[] = {"PASS", "NICK", "USER", "JOIN", "KICK", "INVITE", "CAP", "PING", "MODE", "TOPIC", "PRIVMSG", "PART", "QUIT", "/INVALID"};
    int count = 0;
    size_t arraySize = sizeof(type) / sizeof(type[0]);
    std::cout << "Command : " << _message._command << std::endl;
    for (int i = 0; i < arraySize; i++){
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
            command_nick(server, this->_message);
            break;
        case 2:
            command_user(server, this->_message);
            //TODO check if we need bool or not and check both nickname and user name
            break;
        case 3:
            command_join(server, this->_message);
            break;
        case 4:
            command_kick(server, this->_message);
            break;
        case 5:
            command_invite(server, this->_message);
            break;
        case 6:
            //TODO CAP
            break;
        case 7:
            command_ping(server, this->_message);
            break;
        case 8:
            command_mode(server, this->_message);
            break;
        case 9:
            command_topic(server, this->_message);
            break;
        case 10:
            command_privmsg(server, this->_message);
            break;
        case 11:
            command_part(server, this->_message);
            break;
        case 12:
            command_quit(server, this->_message);
            break;
        case 13:
            std::cout << "Error: invalid command" << std::endl;// check if correct. maybe to put the help ? 
            break;
    }
}

void User::command_pass(Server &server) {
    std::cout << "password function checked" << std::endl;

    if (_message._params.compare(server.get_password()) != 0) {
        send(_fd, ERR_PASSWDMISMATCH, 26, 0);
    }
    else
        _passwordChecked = true;
}

void User::command_nick(Server &server, s_message &message) {
    std::cout << "command_nick function checked" << std::endl;
    if (_passwordChecked == false)
        return;
    static int i = 0;
    std::string new_nick = message._params;
    if (_nick.empty() && (message._params.find(' ') != std::string::npos || message._params.size() > 9))
        new_nick = new_nick.substr(0, 9);
    if (message._params.find(' ') != std::string::npos || message._params.size() > 9) {
        send(_fd, ERR_ERRONEUSNICKNAME(new_nick).c_str(), ERR_ERRONEUSNICKNAME(new_nick).size(), 0);
        return ;
    }
    std::string old = get_nick();
    if (new_nick.empty()) {
        send(_fd, ERR_NONICKNAMEGIVEN(message._command).c_str(), ERR_NONICKNAMEGIVEN(message._command).size(), 0);
        return ;
    }
    std::vector <User *> clients = server.get_clients();
    for (std::vector<User *>::iterator it = clients.begin(); it != clients.end(); it++) {
        if ((*it)->get_nick().compare(new_nick) == 0) {
            //send(_fd, ERR_NICKNAMEISUSE(new_nick).c_str(),ERR_NICKNAMEISUSE(new_nick).size(), 0);
            if (i != 0)
                new_nick = new_nick.substr(0, new_nick.size() - 1);
            new_nick += std::to_string(i);
            i++;
            break;
        }
    }
    if (!_nick.empty())
        send(_fd, NICK(old, new_nick).c_str(), NICK(old, new_nick).size(), 0);
    set_nick(new_nick);
    return ;
}

void User::command_user(Server &server, s_message &message) {
    std::cout << "command_user function checked" << std::endl;
    if (!_passwordChecked || _nick.empty()) {
        delete this; // TODO : check for leaks
        return;
    }
    //format : <username> <hostname> <servername> :<realname>
    std::stringstream ss(message._params);
    std::string word;
    int count = 0;

    while (ss >> word) {
        if (count == 0)
            this->_name = word;
        else if (count == 1)
            this->_hostName = word;
        else if (count == 2)
            this->_serverName = word;
        else if (count == 3 and word[0] == ':')
            this->_realName = word;
        else if (count == 3 and word[0] != ':')
            break;
        else
            this->_realName += " " + word;
        count++;
    }
    if (this->_name.size() > 9) {
        _name = _name.substr(0, 9);
    }
    if (count < 3) {
        send(_fd, ERR_NEEDMOREPARAMS(message._command).c_str(), ERR_NEEDMOREPARAMS(message._command).size() , 0);
        return ;
    }
    std::cout << _nick << std::endl;
    send(_fd, RPL_WELCOME(_nick, _name, _hostName).c_str(), RPL_WELCOME(_nick, _name, _hostName).size(), 0);
    return ;
}

void User::command_ping(Server &server, s_message &message) {
    std::cout << "command_ping function checked" << std::endl;
    if (_message._params.empty()) {
        send(_fd, ERR_NOORIGIN(_message._command).c_str(), ERR_NOORIGIN(_message._command).size(), 0);
        return;
    }
    else
        send(_fd, PONG(_message._params).c_str(), PONG(_message._params).size(), 0);
}

void User::command_join(Server &server, s_message &message) {
    std::cout << "command_join function checked" << std::endl;
    //TODO : check if we have the param we need; #name channel
    int i = 0;
    std::stringstream ss(message._params);
    std::string channelName;
    std::string password;
    std::string word;
    int count = 0;
    while (ss >> word) {
        if (count == 0)
            channelName = word;
        else
            password = word;
        count++;
    }
    for (std::vector<Channel *>::iterator it = server.get_channels().begin(); it != server.get_channels().end(); it++) {
        if ((*it)->get_name() == channelName) {
            if ((*it)->get_limitSet()) {
                if ((*it)->get_userSize() + 1 > (*it)->get_limit()) {
                    send(_fd, ERR_CHANNELISFULL(_nick, channelName).c_str(), ERR_CHANNELISFULL(_nick, channelName).size(), 0);
                    return;
                }
            }
            if ((*it)->get_inviteOnly()) {
                if (this->get_InviteStatus(*it) == false) {
                    send(_fd, ERR_INVITEONLYCHAN(_nick, channelName).c_str(), ERR_INVITEONLYCHAN(_nick, channelName).size(), 0);
                    return;
                }
            }
            if((*it)->get_keySet()) {
                if ((*it)->get_password() != password)
                {
                    send(_fd, ERR_BADCHANNELKEY(_nick, channelName).c_str(), ERR_BADCHANNELKEY(_nick, channelName).size(), 0);
                    return;
                }
            }
            (*it)->add_user(*this);
            set_channel_atm(**it);
            setOperatorStatus(**it, false);
            _channel_rn->send_to_all_macro(JOIN(this->get_nick(), this->get_name(), _hostName, (*it)->get_name()));   
            if (!_channel_rn->get_topic().empty())
                send(_fd, RPL_TOPIC(_nick, _name, _hostName, _channel_rn->get_name(),  _channel_rn->get_topic()).c_str(), RPL_TOPIC(_nick, _name, _hostName, _channel_rn->get_name(), _channel_rn->get_topic()).size(), 0);
            std::string usersInChannel;
            for (std::vector<User *>::iterator it = _channel_rn->get_users().begin(); it != _channel_rn->get_users().end(); it++) {
                if ((*it)->get_operatorStatus(_channel_rn))
                    usersInChannel += "@" +(*it)->get_nick() + " ";
                else
                    usersInChannel += (*it)->get_nick() + " ";
            }
            send(_fd, (RPL_NAMREPLY(_nick, _name, _hostName, _channel_rn->get_name()) + usersInChannel + "\r\n").c_str(), (RPL_NAMREPLY(_nick, _name, _hostName, _channel_rn->get_name()) + usersInChannel + "\r\n").size(), 0);
            send(_fd, RPL_ENDOFNAMES(_nick, _name, _hostName, _channel_rn->get_name()).c_str(), RPL_ENDOFNAMES(_nick, _name, _hostName, _channel_rn->get_name()).size(), 0);
            break;
        } 
        else
            i++;
    }
    if (i == server.get_channels().size()) {
        Channel *channel = new Channel(channelName);
        channel->add_user(*this);
        set_channel_atm(*channel);
        setOperatorStatus(*channel, true);
        std::string usersInChannel = "@" + _nick;
        send(_fd, RPL_YOUREOPER(_nick).c_str(), RPL_YOUREOPER(_nick).size(), 0);
        send(_fd, JOIN(_nick, _name, _hostName, channel->get_name()).c_str(), JOIN(_nick, _name, _hostName, channel->get_name()).size(), 0);
        send(_fd, (RPL_NAMREPLY(_nick, _name, _hostName, _channel_rn->get_name()) + usersInChannel + "\r\n").c_str(), (RPL_NAMREPLY(_nick, _name, _hostName, _channel_rn->get_name()) + usersInChannel + "\r\n").size(), 0);
        send(_fd, RPL_ENDOFNAMES(_nick, _name, _hostName, _channel_rn->get_name()).c_str(), RPL_ENDOFNAMES(_nick, _name, _hostName, _channel_rn->get_name()).size(), 0);
        server.get_channels().push_back(channel);
    }
}

void User::command_topic(Server &server, s_message &message) {
    std::cout << "command_topic function checked" << std::endl;
    if (_isInAChannel)
    {
        if (_channel_rn->get_topicRestricted())
        {
            if (!get_operatorStatus(_channel_rn))
            {
                send(_fd, ERR_CHANOPRIVSNEEDED(_channel_rn->get_name()).c_str(), ERR_CHANOPRIVSNEEDED(_channel_rn->get_name()).size(), 0);
                return;
            }
        }
        std::stringstream ss(message._params);
        std::string word;
        std::string nameTopic;
        int count = 0;
        while (ss >> word) {
            if (count == 1)
            {
                if (word[0] == ':')
                    nameTopic = word.substr(1, word.length());
                else
                    nameTopic = word;
            }
            else
                nameTopic += " " + word;
            count++;
        }
        if (nameTopic.empty())
        {
            if (_channel_rn->get_topic().empty())
                send(_fd, RPL_NOTOPIC(_nick, _name, _hostName, _channel_rn->get_name()).c_str(), 
                RPL_NOTOPIC(_nick, _name, _hostName, _channel_rn->get_name()).size(), 0);
            else
                send(_fd, RPL_TOPIC(_nick, _name, _hostName, _channel_rn->get_name(), _channel_rn->get_topic()).c_str(), 
                RPL_TOPIC(_nick, _name, _hostName, _channel_rn->get_name(), _channel_rn->get_topic()).size(), 0);
        }
        else
        {
            _channel_rn->set_topic(nameTopic);
            _channel_rn->send_to_all_macro(RPL_TOPIC(_nick, _name, _hostName, _channel_rn->get_name(), nameTopic));
        }
    }
    else
    {
        // TODO : it is supposed to send the error message bellow but I'm not in any channel and the message receives by irssi does not contain the channel name
        //send(_fd, ERR_NOTONCHANNEL(_channel_rn->get_name()).c_str(), (ERR_NOTONCHANNEL( _channel_rn->get_name()).size()), 0);
        std::cout << "Cannot use this command in that context" << std::endl;
    }
}

//  format : 
//  * /mode #channel +t => +t (topic): When set, only channel operators can change the channel topic. 
//  * /mode #channel +i => Set/remove Invite-only channel
//  * /mode #channel +k password => k: Set/remove the channel key (password)
//  * /mode #channel +o nickname => o: Give/take channel operator privilege
//  * /mode #channel +l limit => l: Set/remove the user limit to channel

void User::command_mode(Server &server, s_message &message) {
    std::cout << "command_mode function checked" << std::endl;

    std::stringstream ss(message._params);
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
                            send(_fd, ERR_CHANOPRIVSNEEDED(channel).c_str(), ERR_CHANOPRIVSNEEDED(channel).size(), 0);
                            return;
                        }    
                        else {
                            skipped = true;
                            break;
                        } 
                    }
                }
                if (!skipped) {
                    send(_fd, ERR_NOTONCHANNEL(channel).c_str(), ERR_NOTONCHANNEL(channel).size(), 0);
                    return;
                }
                break;
            }
        }
        if (!skipped) {
            std::string test = _nick + " +i";
            if (message._params == test)
                return;
            send(_fd, ERR_NOSUCHCHANNEL(channel).c_str(), ERR_NOSUCHCHANNEL(channel).size(), 0);
            return;
        }
    }
    if (flags != "\0" && _isInAChannel && get_operatorStatus(_channel_rn) == false) {
        send(_fd, ERR_CHANOPRIVSNEEDED(_channel_rn->get_name()).c_str(), ERR_CHANOPRIVSNEEDED(_channel_rn->get_name()).size(), 0);
        return;
    }
    s_flag *parsed;
    if (flags[0] == '+' || flags[0] == '-') {
        parsed = parserOption(flags);
        interpretMode(parsed, optionsArray, (*_channel_rn));
        s_flag *currentFlag = parsed;
        while (currentFlag != nullptr){
            s_flag *nextFlag = currentFlag->next;
            delete currentFlag;
            currentFlag = nextFlag; 
        }
    }
    return;
}

void User::interpretMode(s_flag *parsed, std::vector<std::string> options, Channel &channel)
{
    int i = 0;
    while(parsed != nullptr)
    {
        if (parsed->flag == 't' && parsed->sign == 1)
        {
            if (!channel.get_topicRestricted())
            {
                channel.set_topicRestricted(true);
                channel.send_to_all_macro(RPL_CHANNELMODEIS(_serverName, _nick, channel.get_name(), "+t"));
            }
        }
        else if (parsed->flag == 't' && parsed->sign == 2)
        {
            if (channel.get_topicRestricted())
            {
                channel.set_topicRestricted(false);
                channel.send_to_all_macro(RPL_CHANNELMODEIS(_serverName, _nick, channel.get_name(), "-t"));

            }
        }
        else if (parsed->flag == 'i' && parsed->sign == 1)
        {
            if (!channel.get_inviteOnly())
            {
                channel.set_inviteOnly(true);
                channel.send_to_all_macro(RPL_CHANNELMODEIS(_serverName, _nick, channel.get_name(), "+i"));
            }
        }
        else if (parsed->flag == 'i' && parsed->sign == 2)
        {
            if (channel.get_inviteOnly())
            {
                channel.set_inviteOnly(false);
                channel.send_to_all_macro(RPL_CHANNELMODEIS(_serverName, _nick, channel.get_name(), "-i"));
            }
        }
        else if (parsed->flag == 'l' && parsed->sign == 1)
        {
            try
            {
                int limit = std::stoi(options[i]);
                channel.set_limitSet(true);
                channel.set_limit(limit);
                std::string flags = "+l " + std::to_string(limit);
                channel.send_to_all_macro(RPL_CHANNELMODEIS(_serverName, _nick, channel.get_name(), flags));
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
                channel.send_to_all_macro(RPL_CHANNELMODEIS(_serverName, _nick, channel.get_name(), "-l"));
            }
        }
        else if (parsed->flag == 'k' && parsed->sign == 1)
        {
            try
            {
                channel.set_keySet(true);
                channel.set_password(options[i]);
                std::string flags = "+k " + options[i];
                channel.send_to_all_macro(RPL_CHANNELMODEIS(_serverName, _nick, channel.get_name(), flags));
            }
            catch(const std::exception& e)
            {
                std::cerr << e.what() << '\n'; // correct ?
            }
            i++;
        }
        else if (parsed->flag == 'k' && parsed->sign == 2)
        {
            if (channel.get_keySet())
            {
                channel.set_keySet(false);
                channel.send_to_all_macro(RPL_CHANNELMODEIS(_serverName, _nick, channel.get_name(), "-k"));
            }
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
                    send(_fd,RPL_YOUREOPER(_nick).c_str(), RPL_YOUREOPER(_nick).size(), 0);
                    changed_plus = true;
                    channel.send_to_all_macro(RPL_CHANNELMODEIS(this->_serverName, this->_nick, channel.get_name(), flags));
                    break;
                }
            }
            if (!changed_plus) {
                send(_fd, ERR_USERNOTINCHANNEL(nickname, channel.get_name()).c_str(),
                     ERR_USERNOTINCHANNEL(nickname, channel.get_name()).size(), 0); // TODO
                continue; // ?
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
                    channel.send_to_all_macro(RPL_CHANNELMODEIS(this->_serverName, this->_nick, channel.get_name(), flags));
                    break;
                }
            }
            if (!changed_minus) {
                send(_fd, ERR_USERNOTINCHANNEL(nickname, channel.get_name()).c_str(),
                     ERR_USERNOTINCHANNEL(nickname, channel.get_name()).size(), 0);
                continue;
            }
            i++;
        }
        else
            send(_fd, ERR_UNKNOWNMODE(_serverName, _nick, parsed->flag).c_str(), ERR_UNKNOWNMODE(_serverName, _nick, parsed->flag).size(), 0);
        //TODO : parsing not correct ?
        parsed = parsed->next;
    }
    return;
}

s_flag *User::parserOption(std::string flags)
{
    int i = 0;
    s_flag* head = nullptr;
    s_flag *currentFlag = nullptr;
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
            if (head == nullptr)
            {
                head = newFlag;
                head->prev = nullptr;
                currentFlag = newFlag;
                currentFlag->prev = nullptr;
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
            else if (currentFlag != nullptr && currentFlag->prev != nullptr)
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
    newFlag->next = nullptr;
    return (newFlag);
}

void User::command_privmsg(Server &server, s_message &message) {
    std::cout << "Command privmsg reached" << std::endl;
    std::stringstream ss(message._params);
    std::string word;
    std::string user = "\0";
    std::string msg;
    int count = 0;
    while (ss >> word) {
        if (count == 0 && (!_isInAChannel || (_isInAChannel && word != _channel_rn->get_name())))
            user = word;
        else if (count != 0)
            msg += word + " ";
        count++;
    }
    msg = msg.substr(1); // this takes out the ":" in front of the message
    if (user == "\0")
        _channel_rn->send_to_all_private(msg,this, _nick);
    else
    {
        for (std::vector <User *>::iterator it2 = server.get_clients().begin(); it2 != server.get_clients().end(); it2++)
            if ((*it2)->get_nick() == user)
                send((*it2)->get_fd(), PRIVMSG(_nick, _name, _hostName, user, msg).c_str(), PRIVMSG(_nick, _name, _hostName, user, msg).size(), 0);
    }
}


void User::command_part(Server &server, s_message &message) {
    std::stringstream ss(message._params);
    std::string word;
    std::string msg = "\0";
    int count = 0;
    while (ss >> word) {
        if (count == 1)
            msg = word + " ";
        else if (count != 0)
            msg += word + " ";
        count++;
    }
    if (!_channel_rn) {
        send(_fd, ERR_NEEDMOREPARAMS(message._command).c_str(), ERR_NEEDMOREPARAMS(message._command).size(), 0);
        return;
    }
    if (msg == "\0")
        _channel_rn->send_to_all_macro(PART(_nick, _name, _hostName, _channel_rn->get_name()));
    else
        _channel_rn->send_to_all_macro(PART_REASON(_nick, _name, _hostName, _channel_rn->get_name(), msg));
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
        _channel_rn = nullptr;
        _isInAChannel = false;
    }
    else {
        for (std::map<Channel *, bool>::iterator it = _operatorStatusMap.begin(); it != _operatorStatusMap.end(); ++it)
            _channel_rn = it->first;
        _isInAChannel = true;
    }
}

void User::command_kick(Server &server, s_message &message) {
    std::cout << "command_kick function checked" << std::endl;
    if (!get_operatorStatus(_channel_rn)) {
        send(_fd, ERR_CHANOPRIVSNEEDED(_channel_rn->get_name()).c_str(), ERR_CHANOPRIVSNEEDED(_channel_rn->get_name()).size(), 0);
        return;
    }
    else {
        std::stringstream ss(message._params);
        std::string word;
        std::string userToBeKicked;
        std::string reason;
        int count = 0;
        while (ss >> word) {
            if (count == 1)
                userToBeKicked = word;
            else if (count != 0)
                reason += word + " ";
            count++;
        }
        bool found = false;
        if (!userToBeKicked.empty()) {
            for (std::vector<User *>::iterator it = _channel_rn->get_users().begin(); it != _channel_rn->get_users().end(); it++) {
                if ((*it)->get_nick() == userToBeKicked) {
                    _channel_rn->send_to_all_macro(KICK(_nick, _name, _hostName, _channel_rn->get_name(), userToBeKicked, reason));
                    found = true;
                    break;
                }
            }
            if (!found)
                send(_fd, ERR_USERNOTINCHANNEL(userToBeKicked, _channel_rn->get_name()).c_str(), ERR_USERNOTINCHANNEL(userToBeKicked, _channel_rn->get_name()).size(), 0);
        }
        else
            send(_fd, ERR_NEEDMOREPARAMS(message._command).c_str(), ERR_NEEDMOREPARAMS(message._command).size(), 0);
    }
}

void User::command_invite(Server &server, s_message &message) {
    std::cout << "command_invite function checked" << std::endl;
    if (!get_operatorStatus(_channel_rn)) {
        send(_fd, ERR_CHANOPRIVSNEEDED(_channel_rn->get_name()).c_str(), ERR_CHANOPRIVSNEEDED(_channel_rn->get_name()).size(), 0);
        return;
    }
    else {
        std::stringstream ss(message._params);
        std::string word;
        std::string userToBeInvited;
        std::string channel;
        int count = 0;
        while (ss >> word) {
            if (count == 0)
                userToBeInvited = word;
            else if (count == 1)
                channel = word;
            count++;
        }
        bool found = false;
        if (!userToBeInvited.empty()) {
            for (std::vector<User *>::iterator it = server.get_clients().begin(); it != server.get_clients().end(); it++) {
                if ((*it)->get_nick() == userToBeInvited) {
                    for (std::vector<User *>::iterator it = _channel_rn->get_users().begin(); it != _channel_rn->get_users().end(); it++) {
                        if ((*it)->get_nick() == userToBeInvited) {
                    send(_fd, ERR_USERONCHANNEL(_nick, userToBeInvited, channel).c_str(), ERR_USERONCHANNEL(_nick, userToBeInvited, channel).size(), 0);
                    return;
                }
            }
                    send((*it)->get_fd(), INVITE(_nick, _name, _hostName, userToBeInvited, channel).c_str(), INVITE(_nick, _name, _hostName, userToBeInvited, channel).size(), 0);
                    found = true;
                    (*it)->setInviteStatus(*_channel_rn, true);
                    return;
                }
            }
            if (!found)
                send(_fd, ERR_NOSUCHNICK(userToBeInvited).c_str(), ERR_NOSUCHNICK(userToBeInvited).size(), 0);
        }
        else
            send(_fd, ERR_NEEDMOREPARAMS(message._command).c_str(), ERR_NEEDMOREPARAMS(message._command).size(), 0);
    }


}

void User::command_quit(Server &server, s_message &message) {
    std::cout << "command_quit function checked" << std::endl;
    server.print_channels();
    Channel *channel_ptr;
    for (std::vector<Channel *>::iterator it = server.get_channels().begin(); it != server.get_channels().end(); ++it) {
        for (std::vector<User *>::iterator it2 = (*it)->get_users().begin(); it2 != (*it)->get_users().end(); it2++) {
            if ((*it2)->get_nick() == _nick) {
                if (message._params.empty())
                    (*it)->send_to_all_macro(PART(_nick, _name, _hostName, (*it)->get_name()));
                else
                    (*it)->send_to_all_macro(PART_REASON(_nick, _name, _hostName, (*it)->get_name(), message._params));
                (*it)->remove_user(*this);
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
    }
    if (channel_ptr != nullptr) {
        std::vector<Channel *>::iterator it = std::find(server.get_channels().begin(), server.get_channels().end(),
                                                        channel_ptr);
        if (it != server.get_channels().end()) {
            server.get_channels().erase(it);
            delete channel_ptr;
        }
        send(_fd, QUIT(_nick, _name, _hostName).c_str(), QUIT(_nick, _name, _hostName).size(), 0);
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
}


/*irc interpretation : 
* /mode +t +l ==> send "+tl"
* /mode +t +l 10 +o mimi ==> send #CHANNEL +tlo 10 mimi
* /mode +t -l 10 +o mimi ==> send +t-l10+o mimi
* /mode +t -l 58 +k hello ==> send +t-l58+k hello
* /mode +t +l 58 +k hello ==> send +tlk 58 hello
* /mode +t +l 58 -k hello ==> send +tl-k 58 hello 
* /mode +t +l 58 -k hello +o ==> send +tl-k+o 58 hello
*/


