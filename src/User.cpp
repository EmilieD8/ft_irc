#include "../inc/irc.hpp"

User::User() {
    _isInAChannel = false;
    _channel_rn = NULL;
}

User::User(int fd, int id) : _fd(fd), id(id)  {
    _isInAChannel = false;
    _channel_rn = NULL;
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

std::string User::get_pw() const {
    return _pw;
}

void User::set_channel_atm(Channel& channel) {
    _channel_rn = &channel;
    _isInAChannel = true;
}

void User::setOperatorStatus(Channel *channel, bool isOperator)
{
    _operatorStatusMap[channel] = isOperator;
}

bool User::get_operatorStatus(Channel* channel) const {
    std::map<Channel*, bool>::const_iterator it = _operatorStatusMap.find(channel);
    return (it != _operatorStatusMap.end()) ? it->second : false;
}

void User::set_nick(std::string nick) {
    _nick = nick;
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
    parseMessage(server);
}

void User::parseMessage(Server &server) {
    std::string type[] = {"PASS", "NICK", "USER", "JOIN", "KICK", "INVITE", "CAP", "PING", "MODE", "TOPIC", "/INVALID"};
    //   TODO : replace also the command by their digit codes
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
            passwordCheck(server);
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
            //TODO KIC
            break;
        case 5:
            //TODO INVITE
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
            std::cout << "Error: invalid command" << std::endl;// check if correct. maybe to put the help ? 
            break;
    }
}

void User::passwordCheck(Server &server) {
    std::cout << "password function checked" << std::endl;

    if (_message._params.compare(server.get_password()) != 0) {
        send(_fd, ERR_PASSWDMISMATCH, 26, 0);
        server.set_exit_status(true);
        throw std::runtime_error("Error wrong password");
    }
    else
        std::cout << "password is correct" << std::endl;
    // TODO: maybe create a while loop to give three more tries to connect with the right password
}

bool User::command_nick(Server &server, s_message &message) {
    std::cout << "command_nick function checked" << std::endl;
    // TODO : if user already taken at first instance, then change the username with a special symbol.
    //TODO : check if nickname changes works
    //TODO : chekc if proper error messages sent
    std::string new_nick = message._params;
    std::string old = get_nick();
        if (new_nick.empty()) {
            send(_fd, ERR_NONICKNAMEGIVEN(message._command).c_str(), ERR_NONICKNAMEGIVEN(message._command).size(), 0);
            return false;
        }
        std::vector <User> clients = server.get_clients();
        for (std::vector<User>::iterator it = clients.begin(); it != clients.end(); it++) {
            if (it->get_nick().compare(new_nick) == 0) {
                send(_fd, ERR_NICKNAMEINUSE(message._command, new_nick).c_str(),
                     ERR_NICKNAMEINUSE(message._command, new_nick).size(), 0);
                return false;
            }
        }
    set_nick(new_nick);
    send(_fd, NICK(old, new_nick).c_str(), NICK(old, new_nick).size(), 0);
    return true;
}

bool User::command_user(Server &server, s_message &message) {
    std::cout << "command_user function checked" << std::endl;
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
    if (count < 4) {
        send(_fd, ERR_NEEDMOREPARAMS(message._command).c_str(), ERR_NEEDMOREPARAMS(message._command).size() , 0);
        return false;
    }
    send(_fd, RPL_WELCOME(_nick, _name, _hostName).c_str(), RPL_WELCOME(_nick, _name, _hostName).size(), 0);
    return true;
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
    int i = 0;
    for (std::vector<Channel>::iterator it = server.get_channels().begin(); it != server.get_channels().end(); it++) {
        if (it->get_name() == message._params) {
            std::cout << "Channel exists already" << std::endl;
            if ((*it).get_limitSet() == true)
            {
                if ((*it).get_userSize() > (*it).get_limit())
                {
                    send(_fd, ERR_CHANNELISFULL(_nick, _channel_rn->get_name()).c_str(), ERR_CHANNELISFULL(_nick, _channel_rn->get_name()).size(), 0);
                    return;
                }
            }
            it->add_user(*this);
            set_channel_atm(*it);
            setOperatorStatus(&(*it), false);
            send(_fd, JOIN(this->get_nick(), this->get_name(), _hostName, it->get_name()).c_str(),
                JOIN(this->get_nick(), this->get_name(), _hostName, it->get_name()).size(), 0);
            send(_fd, RPL_TOPIC(_nick, _name, _hostName, _channel_rn->get_name(),  _channel_rn->get_topic()).c_str(), RPL_TOPIC(_nick, _name, _hostName, _channel_rn->get_name(), _channel_rn->get_topic()).size(), 0);
            //DEBUG
            std::cout << "Number of user in this channel :" << it->get_users().size() << std::endl;
            break;
        } 
        else
            i++;
    }
    if (i == server.get_channels().size())
    {
        std::cout << "Channel does not exist already" << std::endl;
        Channel *channel = new Channel(message._params);
        channel->add_user(*this);
        server.get_channels().push_back(*channel);
        set_channel_atm(*channel);
        setOperatorStatus(channel, true);
        send(_fd, JOIN(this->get_nick(), this->get_name(), _hostName, channel->get_name()).c_str(),
                JOIN(this->get_nick(), this->get_name(), _hostName, channel->get_name()).size(), 0);
    }
    //server.print_channels();
    
}

void User::command_topic(Server &server, s_message &message) {
    std::cout << "command_topic function checked" << std::endl;
std::cout << " fd is " << _fd << "and _isInChannel is " << _isInAChannel << "and is operator is " 
<< this->get_operatorStatus(_channel_rn) << std::endl;
/*
 TODO:
 format : TOPIC #channel :New Topic 
- check if the user is in that channel atm otherwise fail - error ERR_NOTONCHANNEL 
- check if the flag mode -t is set otherwise no if user is not operator
- change the topic and send a message
- if does not contain a param <topic>, then just return the current topic of the channel.
RPL_NOTOPIC
                        "<channel> :No topic is set"
        332     RPL_TOPIC
                        "<channel> :<topic>"

                - When sending a TOPIC message to determine the
                  channel topic, one of two replies is sent.  If
                  the topic is set, RPL_TOPIC is sent back else
                  RPL_NOTOPIC.

*/
    if (_isInAChannel == true)
    {
        if (_channel_rn->get_topicRestricted() == true)
        {
            std::cout << "topic is restricted" << std::endl; // to be deleted
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
            send(_fd, RPL_TOPIC(_nick, _name, _hostName, _channel_rn->get_name(), nameTopic).c_str(), 
                RPL_TOPIC(_nick, _name, _hostName, _channel_rn->get_name(), nameTopic).size(), 0);
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
        std::cout << "Cannot use this command in that context" << std::endl;
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
    
    //after, delete all the flag struct
}
        // s_flag *currentFlag = parsed;
        // while (currentFlag != nullptr)
        // {
        //     std::cout << "Flag: " << currentFlag->flag << ", Sign: " << currentFlag->sign << 
        //     ", isValid : " << currentFlag->isValid << std::endl;
        //     currentFlag = currentFlag->next;
        // }
// unknown mode flag :         472     ERR_UNKNOWNMODE => let's return the list of mode that we can write
//         221     RPL_UMODEIS

void User::interpretMode(s_flag *parsed, std::vector<std::string> options)
{
    int i = 0; // iterator for the vector;
    while(parsed != nullptr)
    {
        std::cout << "Flag: " << parsed->flag << ", Sign: " << parsed->sign << std::endl;
        if (parsed->flag == 't' && parsed->sign == 1)
        {
            if (_channel_rn->get_topicRestricted() == false)
            {
                _channel_rn->set_topicRestricted(true);
                std::cout << "flag changed +t" << std::endl;
                // TODO : does not send the message to the server
                send(_fd, MODE_CHANNELMSGWITHPARAM( _channel_rn->get_name(), parsed->flag, parsed->flag).c_str(),MODE_CHANNELMSGWITHPARAM(_channel_rn->get_name(), parsed->flag, parsed->flag).size(), 0);
            }
        }
        else if (parsed->flag == 't' && parsed->sign == 2)
        {
            if (_channel_rn->get_topicRestricted() == true)
            {
                _channel_rn->set_topicRestricted(false);
                std::cout << "flag changed -t" << std::endl;
                // TODO : does not send the message to the server
                send(_fd, MODE_CHANNELMSGWITHPARAM( _channel_rn->get_name(), parsed->flag, parsed->flag).c_str(),MODE_CHANNELMSGWITHPARAM(_channel_rn->get_name(), parsed->flag, parsed->flag).size(), 0);
            }
        }
        else if (parsed->flag == 'i' && parsed->sign == 1)
        {
            if (_channel_rn->get_inviteOnly() == false)
            {
                _channel_rn->set_inviteOnly(true);
                std::cout << "flag changed +i" << std::endl;
                // TODO : does not send the message to the server
                send(_fd, MODE_CHANNELMSGWITHPARAM( _channel_rn->get_name(), parsed->flag, parsed->flag).c_str(),MODE_CHANNELMSGWITHPARAM(_channel_rn->get_name(), parsed->flag, parsed->flag).size(), 0);
            }
        }
        else if (parsed->flag == 'i' && parsed->sign == 2)
        {
            if (_channel_rn->get_inviteOnly() == true)
            {
                _channel_rn->set_inviteOnly(false);
                std::cout << "flag changed -i" << std::endl;
                // TODO : does not send the message to the server
                send(_fd, MODE_CHANNELMSGWITHPARAM( _channel_rn->get_name(), parsed->flag, parsed->flag).c_str(),MODE_CHANNELMSGWITHPARAM(_channel_rn->get_name(), parsed->flag, parsed->flag).size(), 0);
            }
        }



        else if (parsed->flag == 'l' && parsed->sign == 1)
        {
            
            if (_channel_rn->get_inviteOnly() == false)
            {
                _channel_rn->set_inviteOnly(true);
                std::cout << "flag changed +i" << std::endl;
                // TODO : does not send the message to the server
                send(_fd, MODE_CHANNELMSGWITHPARAM( _channel_rn->get_name(), parsed->flag, parsed->flag).c_str(),MODE_CHANNELMSGWITHPARAM(_channel_rn->get_name(), parsed->flag, parsed->flag).size(), 0);
            }
            i++;
        }
        else if (parsed->flag == 'i' && parsed->sign == 2)
        {
            if (_channel_rn->get_inviteOnly() == true)
            {
                _channel_rn->set_inviteOnly(false);
                std::cout << "flag changed -i" << std::endl;
                // TODO : does not send the message to the server
                send(_fd, MODE_CHANNELMSGWITHPARAM( _channel_rn->get_name(), parsed->flag, parsed->flag).c_str(),MODE_CHANNELMSGWITHPARAM(_channel_rn->get_name(), parsed->flag, parsed->flag).size(), 0);
            }
            i++;
        }







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

/*irc interpretation : 
* /mode +t +l ==> send "+tl"
* /mode +t +l 10 +o mimi ==> send +tlo 10 mimi
* /mode +t -l 10 +o mimi ==> send +t-l10+o mimi
* / mode +t -l 58 +k hello ==> send +t-l58+k hello
* /mode +t +l 58 +k hello ==> send +tlk 58 hello
* /mode +t +l 58 -k hello ==> send +tl-k 58 hello 
* /mode +t +l 58 -k hello +o ==> send +tl-k+o 58 hello 

*/