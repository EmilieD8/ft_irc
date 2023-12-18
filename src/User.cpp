#include "../inc/irc.hpp"

User::User() {
    _isInAChannel = false;
    _channel_rn = NULL;
    _nick = "\0";
    _nickOp = "\0";
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
    _nickOp = "\0";
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

std::string User::get_nickOp() const {
    return _nickOp;
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

void User::set_nickOp(std::string nickOp) {
    _nickOp = nickOp;
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
    std::string type[] = {"PASS", "NICK", "USER", "JOIN", "KICK", "INVITE", "CAP", "PING", "MODE", "TOPIC", "PRIVMSG", "PART", "/INVALID"};
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
            std::cout << "Error: invalid command" << std::endl;// check if correct. maybe to put the help ? 
            break;
    }
}

void User::command_pass(Server &server) {
    std::cout << "password function checked" << std::endl;

    if (_message._params.compare(server.get_password()) != 0) {
        send(_fd, ERR_PASSWDMISMATCH, 26, 0);
        //server.set_exit_status(true);
        //throw std::runtime_error("Error wrong password");
    }
    else
        _passwordChecked = true;
    // TODO: maybe create a while loop to give three more tries to connect with the right password
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
        send(_fd, ERR_ERRONEUSNICKNAME(_nick).c_str(), ERR_ERRONEUSNICKNAME(_nick).size(), 0);
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
            send(_fd, ERR_NICKNAMEISUSE(new_nick).c_str(),ERR_NICKNAMEISUSE(new_nick).size(), 0);
            new_nick = new_nick.substr(0, new_nick.size() - 1);
            new_nick += std::to_string(i);
            i++;
            set_nick(new_nick);
            send(_fd, NICK(old, new_nick).c_str(), NICK(old, new_nick).size(), 0);
            return ;
        }
    }
    std::string newNickOp = "@" + new_nick; // are we still using it ?
    set_nick(new_nick);
    set_nickOp(newNickOp);
    send(_fd, NICK(old, new_nick).c_str(), NICK(old, new_nick).size(), 0);
    return ;
}

void User::command_user(Server &server, s_message &message) {
    std::cout << "command_user function checked" << std::endl;
    if (_passwordChecked == false || _nick.empty() == true) {
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


/*
 * TODO:
 * check if channel already exists or otherwise create it
 * proper name? Test vs #Test
 * Test if a user is already in that channel -> should not be added twice
 * different modes ONLY FOR CHANNELS OPERATORS:
 * format : 
 * /mode #channel +t => +t (topic): When set, only channel operators can change the channel topic.
 * /mode #channel +i => Set/remove Invite-only channel
 * /mode #channel +k password => k: Set/remove the channel key (password)
 * /mode #channel +o nickname => o: Give/take channel operator privilege
 * /mode #channel +l limit => l: Set/remove the user limit to channel
 */

void User::command_join(Server &server, s_message &message) {
    std::cout << "command_join function checked" << std::endl;
    //TODO : check if we have the param we need;
    int i = 0;
    for (std::vector<Channel *>::iterator it = server.get_channels().begin(); it != server.get_channels().end(); it++) {
        if ((*it)->get_name() == message._params) {
            if ((*it)->get_limitSet() == true) {
                if ((*it)->get_userSize() > (*it)->get_limit()) {
                    send(_fd, ERR_CHANNELISFULL(_nick, _channel_rn->get_name()).c_str(), ERR_CHANNELISFULL(_nick, _channel_rn->get_name()).size(), 0);
                    return;
                }
            }
            if ((*it)->get_inviteOnly() == true) {
                if (this->get_InviteStatus(*it) == false) {
                    send(_fd, ERR_INVITEONLYCHAN(_nick, message._params).c_str(), ERR_INVITEONLYCHAN(_nick, message._params).size(), 0);
                    return;
                }
            }
            (*it)->add_user(*this);
            set_channel_atm(**it);
            setOperatorStatus(**it, false);
            send(_fd, JOIN(this->get_nick(), this->get_name(), _hostName, (*it)->get_name()).c_str(),
                JOIN(this->get_nick(), this->get_name(), _hostName, (*it)->get_name()).size(), 0);
            if (_channel_rn->get_topic().empty() == false)
                send(_fd, RPL_TOPIC(_nick, _name, _hostName, _channel_rn->get_name(),  _channel_rn->get_topic()).c_str(), RPL_TOPIC(_nick, _name, _hostName, _channel_rn->get_name(), _channel_rn->get_topic()).size(), 0);
            break;
        } 
        else
            i++;
    }
    if (i == server.get_channels().size()) {
        Channel *channel = new Channel(message._params);
        channel->add_user(*this);
        set_channel_atm(*channel);
        setOperatorStatus(*channel, true);
        send(_fd, RPL_YOUREOPER(_nick).c_str(), RPL_YOUREOPER(_nick).size(), 0);
// TODO: need to add @ to the nickname
        send(_fd, JOIN(_nick, _name, _hostName, channel->get_name()).c_str(),
                JOIN(_nick, _name, _hostName, channel->get_name()).size(), 0);
        server.get_channels().push_back(channel);
    }
}

void User::command_topic(Server &server, s_message &message) {
    std::cout << "command_topic function checked" << std::endl;
    if (_isInAChannel == true)
    {
        if (_channel_rn->get_topicRestricted() == true)
        {
            if (get_operatorStatus(_channel_rn) == false)
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

    if (_isInAChannel == false)
    {
        //is not in a channel, cannot use this command;
        std::cout << "Cannot use this command in that context" << std::endl; //TODO :
        return;
    }
    if (get_operatorStatus(_channel_rn) == false)
    {
        send(_fd, ERR_CHANOPRIVSNEEDED(_channel_rn->get_name()).c_str(), ERR_CHANOPRIVSNEEDED(_channel_rn->get_name()).size(), 0);
        return;
    }
    std::stringstream ss(message._params);
    std::string word;
    std::string flags;
    std::vector<std::string> optionsArray;
    int count = 0;
    while (ss >> word) {
        if (count == 0 && word.compare(_channel_rn->get_name()) != 0)
            flags = word;
        if (count == 1)
            flags += word;
        else if (count != 0)
            optionsArray.push_back(word);
        count++;
    }
    //parser
    s_flag *parsed;
    if (flags[0] == '+' || flags[0] == '-')
        parsed = parserOption(flags);
    else
    {
        send(_fd, ERR_NEEDMOREPARAMS(message._command).c_str(), ERR_NEEDMOREPARAMS(message._command).size(), 0);
        return;
    }
    interpretMode(parsed, optionsArray);
    
    s_flag *currentFlag = parsed;
    while (currentFlag != nullptr)
    {
        s_flag *nextFlag = currentFlag->next;
        delete currentFlag;
        currentFlag = nextFlag; 
    }
    return;
}

// unknown mode flag :         472     ERR_UNKNOWNMODE => let's return the list of mode that we can write
//         221     RPL_UMODEIS

void User::interpretMode(s_flag *parsed, std::vector<std::string> options)
{
    int i = 0; // iterator for the vector;
    while(parsed != nullptr)
    {
        if (parsed->flag == 't' && parsed->sign == 1)
        {
            if (_channel_rn->get_topicRestricted() == false)
            {
                _channel_rn->set_topicRestricted(true);
                _channel_rn->send_to_all_macro(RPL_CHANNELMODEIS(_serverName, _nick, _channel_rn->get_name(), "+t"));
            }
        }
        else if (parsed->flag == 't' && parsed->sign == 2)
        {
            if (_channel_rn->get_topicRestricted() == true)
            {
                _channel_rn->set_topicRestricted(false);
                _channel_rn->send_to_all_macro(RPL_CHANNELMODEIS(_serverName, _nick, _channel_rn->get_name(), "-t"));

            }
        }
        else if (parsed->flag == 'i' && parsed->sign == 1)
        {
            if (_channel_rn->get_inviteOnly() == false)
            {
                _channel_rn->set_inviteOnly(true);
                _channel_rn->send_to_all_macro(RPL_CHANNELMODEIS(_serverName, _nick, _channel_rn->get_name(), "+i"));
            }
        }
        else if (parsed->flag == 'i' && parsed->sign == 2)
        {
            if (_channel_rn->get_inviteOnly() == true)
            {
                _channel_rn->set_inviteOnly(false);
                _channel_rn->send_to_all_macro(RPL_CHANNELMODEIS(_serverName, _nick, _channel_rn->get_name(), "-i"));
            }
        }
        else if (parsed->flag == 'l' && parsed->sign == 1)
        {
            try
            {
                int limit = std::stoi(options[i]);
                _channel_rn->set_limitSet(true);
                _channel_rn->set_limit(limit);
                std::string flags = "+l " + std::to_string(limit);
                _channel_rn->send_to_all_macro(RPL_CHANNELMODEIS(_serverName, _nick, _channel_rn->get_name(), flags));
            }
            catch(const std::exception& e)
            {
                std::cerr << e.what() << '\n';
            }
            i++;
        }
        else if (parsed->flag == 'l' && parsed->sign == 2)
        {
            if (_channel_rn->get_limitSet() == true)
            {
                _channel_rn->set_limitSet(false);
                _channel_rn->send_to_all_macro(RPL_CHANNELMODEIS(_serverName, _nick, _channel_rn->get_name(), "-l"));
            }
        }
        else if (parsed->flag == 'k' && parsed->sign == 1)
        {
            try
            {
                _channel_rn->set_keySet(true);
                _channel_rn->set_password(options[i]);
                std::string flags = "+k " + options[i];
                _channel_rn->send_to_all_macro(RPL_CHANNELMODEIS(_serverName, _nick, _channel_rn->get_name(), flags));
            }
            catch(const std::exception& e)
            {
                std::cerr << e.what() << '\n'; // correct ?
            }
            i++;
        }
        else if (parsed->flag == 'k' && parsed->sign == 2)
        {
            if (_channel_rn->get_keySet() == true)
            {
                _channel_rn->set_keySet(false);
                _channel_rn->send_to_all_macro(RPL_CHANNELMODEIS(_serverName, _nick, _channel_rn->get_name(), "-k"));
            }
        }
        else if (parsed->flag == 'o' && parsed->sign == 1)
        {
            std::string nickname = options[i];
            std::string flags = "+o " + options[i];
            bool changed_plus = false;
            for (std::vector<User *>::iterator it = _channel_rn->get_users().begin(); it != _channel_rn->get_users().end(); it++)
            {
                if ((*it)->get_nick() == nickname)
                {
                    (*it)->setOperatorStatus(*_channel_rn, true);
                    send(_fd,RPL_YOUREOPER(_nick).c_str(), RPL_YOUREOPER(_nick).size(), 0);
                    changed_plus = true;
                    _channel_rn->send_to_all_macro(RPL_CHANNELMODEIS(this->_serverName, this->_nick, _channel_rn->get_name(), flags));
                    break;
                }
            }
            if (changed_plus == false) {
                send(_fd, ERR_USERNOTINCHANNEL(nickname, _channel_rn->get_name()).c_str(),
                     ERR_USERNOTINCHANNEL(nickname, _channel_rn->get_name()).size(), 0); // TODO
                continue;
            }
            i++;
        }
        else if (parsed->flag == 'o' && parsed->sign == 2)
        {
            std::string nickname = options[i];
            std::string flags = "-o " + options[i];
            bool changed_minus = false;
            for (std::vector<User *>::iterator it = _channel_rn->get_users().begin(); it != _channel_rn->get_users().end(); it++)
            {
                if ((*it)->get_nick() == nickname)
                {
                    (*it)->setOperatorStatus(*_channel_rn, false);
                    changed_minus = true;
                    _channel_rn->send_to_all_macro(RPL_CHANNELMODEIS(this->_serverName, this->_nick, _channel_rn->get_name(), flags));
                    break;
                }
            }
            if (changed_minus == false) {
                send(_fd, ERR_USERNOTINCHANNEL(nickname, _channel_rn->get_name()).c_str(),
                     ERR_USERNOTINCHANNEL(nickname, _channel_rn->get_name()).size(), 0);
                // TODO
                continue;
            }
            i++;
        }
        else
        {
            send(_fd, ERR_UMODEUNKNOWNFLAG(this->_nick).c_str(), ERR_UMODEUNKNOWNFLAG(this->_nick).size(), 0);
            //TODO
        }
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
    std::string user;
    std::string msg;
    int count = 0;
    while (ss >> word) {
        if (count == 1 && word[0] != ':')
            user = word;
        else if (count != 0)
            msg += word + " ";
        count++;
    }
    if (user.empty() == true)
    {
        if (get_operatorStatus(_channel_rn) == false)
            _channel_rn->send_to_all_private(message._params, this,_nick);
        else
            _channel_rn->send_to_all_private(message._params,this, _nickOp);
    }
    else
    {
        for (std::vector<User *>::iterator it = _channel_rn->get_users().begin(); it != _channel_rn->get_users().end(); it++)
        {
            if ((*it)->get_nick() == user)
            {
                if (get_operatorStatus(_channel_rn) == false)
                {
                    send((*it)->get_fd(), PRIVMSG(_nick, _name, _hostName, _channel_rn->get_name(), msg).c_str(), PRIVMSG(_nick, _name, _hostName, _channel_rn->get_name(), msg).size(), 0);
                    send(_fd, PRIVMSG(_nick, _name, _hostName, _channel_rn->get_name(), msg).c_str(), PRIVMSG(_nick, _name, _hostName, _channel_rn->get_name(), msg).size(), 0);
                }
                else
                {
                    send(_fd, PRIVMSG(_nickOp, _name, _hostName, _channel_rn->get_name(), msg).c_str(), PRIVMSG(_nickOp, _name, _hostName, _channel_rn->get_name(), msg).size(), 0);
                    send((*it)->get_fd(), PRIVMSG(_nickOp, _name, _hostName, _channel_rn->get_name(), msg).c_str(), PRIVMSG(_nickOp, _name, _hostName, _channel_rn->get_name(), msg).size(), 0);
                }
                return;
            }
        }
    }
}

void User::command_part(Server &server, s_message &message) {
    if (!_channel_rn) {
        send(_fd, ERR_USERNOTINCHANNEL(_nick, message._params).c_str(), ERR_USERNOTINCHANNEL(_nick, message._params).size(), 0);
        return;
    }
    send(_fd, PART(_nick, _name, _hostName, _channel_rn->get_name()).c_str(), PART(_nick, _name, _hostName, _channel_rn->get_name()).size(), 0);
    _channel_rn->remove_user(*this);
    _channel_rn = NULL;
    _isInAChannel = false;
    setInviteStatus(*_channel_rn, false);
    //TODO : need to check if the user is in the channel or not
    //TODO : lose operator status if the user is operator
}

void User::command_kick(Server &server, s_message &message) {
    std::cout << "command_kick function checked" << std::endl;
    if (get_operatorStatus(_channel_rn) == false) {
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
        if (userToBeKicked.empty() == false) {
            for (std::vector<User *>::iterator it = _channel_rn->get_users().begin(); it != _channel_rn->get_users().end(); it++) {
                if ((*it)->get_nick() == userToBeKicked) {
                    _channel_rn->send_to_all_macro(KICK(_nick, _name, _hostName, _channel_rn->get_name(), userToBeKicked, reason));
                    found = true;
                    break;
                }
            }
            if (found == false)
                send(_fd, ERR_USERNOTINCHANNEL(userToBeKicked, _channel_rn->get_name()).c_str(), ERR_USERNOTINCHANNEL(userToBeKicked, _channel_rn->get_name()).size(), 0);
        }
        else
            send(_fd, ERR_NEEDMOREPARAMS(message._command).c_str(), ERR_NEEDMOREPARAMS(message._command).size(), 0);
    }
}

void User::command_invite(Server &server, s_message &message) {
    std::cout << "command_invite function checked" << std::endl;
    if (get_operatorStatus(_channel_rn) == false) {
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
        if (userToBeInvited.empty() == false) {
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
            if (found == false)
                send(_fd, ERR_NOSUCHNICK(userToBeInvited).c_str(), ERR_NOSUCHNICK(userToBeInvited).size(), 0);
        }
        else
            send(_fd, ERR_NEEDMOREPARAMS(message._command).c_str(), ERR_NEEDMOREPARAMS(message._command).size(), 0);
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
