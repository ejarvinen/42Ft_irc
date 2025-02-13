#include "Channel.hpp"

Channel::Channel() : _name(""), _topic(""), _clientLimit(-1), _inviteOnly(false), _topicRestricted(true), _key(""), _topicUpdated(0), _editer("")
{}

Channel::Channel(std::string name, int op, std::string key) : _name(name), _topic(""), _clientLimit(-1), _inviteOnly(false), _topicRestricted(true), _key(key)
{
	_ops.push_back(op);
}

Channel::Channel(const Channel &other) : _name(other.getChannelName()), _topic(other.getTopic()), _clientLimit(other.getClientLimit()), _inviteOnly(other.isInviteOnly()), _topicRestricted(other.isTopicRestricted()), _key(other.getKey()) 
{
	_ops.assign(other._ops.begin(), other._ops.end());
	_jointClients.assign(other._jointClients.begin(), other._jointClients.end());
	_invitedClients.assign(other._invitedClients.begin(), other._invitedClients.end());
	_editer = other.getEditer();
	_topicUpdated = other.getTopicUpdated();
}

Channel	&Channel::operator=(const Channel &other)
{
	if (this != &other)
	{
		_name = other.getChannelName();
		_topic = other.getTopic();
		_ops.assign(other._ops.begin(), other._ops.end());
		_clientLimit = other.getClientLimit();
		_jointClients.assign(other._jointClients.begin(), other._jointClients.end());
		_invitedClients.assign(other._invitedClients.begin(), other._invitedClients.end());
		_inviteOnly = other.isInviteOnly();
		_topicRestricted = other.isTopicRestricted();
		_key = other.getKey();
		_editer = other.getEditer();
		_topicUpdated = other.getTopicUpdated();
	}
	return (*this);
}

Channel::~Channel() {}

void Channel::removeClient(int cfd) {
	std::vector<int>::iterator ite = std::find(_jointClients.begin(), _jointClients.end(), cfd);

    if (ite != _jointClients.end()) {
        _jointClients.erase(ite);
        return ;
    }
    //if clients was not in the _jointClients list, look thorugh the ops list
    removeOp(cfd);
}

void Channel::addToInvitedClients(int cfd) {
	for (size_t i = 0; i < _invitedClients.size(); i++) {
		if (_invitedClients[i] == cfd) {
			return ;
		}
	}
	_invitedClients.push_back(cfd);
}

void Channel::removeOp(int cfd) {

    std::vector<int>::iterator ite = std::find(_ops.begin(), _ops.end(), cfd);

    if (ite != _ops.end()) {
        _ops.erase(ite);
    }
}

void	Channel::removeInvite(int cfd) {

	std::vector<int>::iterator ite = std::find(_invitedClients.begin(), _invitedClients.end(), cfd);

    if (ite != _invitedClients.end()) {
        _invitedClients.erase(ite);
    }
}

void Channel::broadcast(const std::string& msg, int senderFd, bool excludeSender) {

    if (excludeSender) {
		if (!_ops.empty()) {
			for (size_t i = 0; i < _ops.size(); i++) {
				send(_ops[i], msg.c_str(), msg.length(), 0);
			}
		}
		if (!_jointClients.empty()) {
			for (size_t i = 0; i < _jointClients.size(); i++) {
				send(_jointClients[i], msg.c_str(), msg.length(), 0);
			}
		}
    } else {
		if (!_ops.empty()) {
			for (size_t i = 0; i < _ops.size(); i++) {
				if (_ops[i] != senderFd)
					send(_ops[i], msg.c_str(), msg.length(), 0);
			}
		}
		if (!_jointClients.empty()) {
			for (size_t i = 0; i < _jointClients.size(); i++) {
				if (_jointClients[i] != senderFd)
					send(_jointClients[i], msg.c_str(), msg.length(), 0);
			}
		}
    }
}

bool	Channel::containSender(int cfd) const 
{
    if (std::find(_jointClients.begin(), _jointClients.end(), cfd) != _jointClients.end())
        return (true);
    if (std::find(_ops.begin(), _ops.end(), cfd) != _ops.end())
        return (true);
    return (false);
}