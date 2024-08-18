#include "Server.hpp"

void Server::pass(std::vector<std::string> &tokens, int fd)
{
	std::vector<std::string>::iterator tokens_it = tokens.begin();
	std::vector<Client>::iterator client_it = findClient(fd);

	if(tokens.size() == 2)
	{
		if (client_it->is_auth == true)
			return;
		if(*(tokens_it + 1) != this->passwd)
			sendReply(PASS_ERR(), fd);
		else
			client_it->is_auth = true;
	}
	else
		sendReply("Command form is: PASS <password>", fd);
}

void Server::nick(std::vector<std::string> &tokens, int fd)
{
	std::vector<std::string>::iterator tokens_it = tokens.begin();
	std::vector<Client>::iterator client_it = findClient(fd);
	
	if (tokens.size() == 2)
	{
		if (!isAlNumStr(*(tokens_it + 1)))
			sendReply(ERR_NICK(*(tokens_it + 1)),fd);
		else if (findClientNick(*(tokens_it + 1)) != clients.end())
			sendReply(ERR_ALREADYREGISTERED(*(tokens_it + 1)),fd);
		else
		{

            client_it->setNick(*(tokens_it + 1));
			//channel içindeki nicki de değiştir
			//fd atıp channel içindeki clientı bul ve chanel içinde adını değiştir
		}
	}
	else
		sendReply("Command form is: NICK <nickname>", fd);
}
void Server::quit(std::vector<std::string> &tokens, int fd)
{
    if (tokens.size() == 1)
    {
        FD_CLR(fd, &current);
        close(fd);
        clients.erase(findClient(fd)); //bunun yerine full silme kullanılıcak chanellardan falan
        std::cout << "Client " << findClient(fd)->getId() << " left the server" << std::endl;
    }
    else
	{
		sendReply("Command form is: QUIT", fd);
	}
	//quit komutuna mesaj eklemem gerekli mi
}



void	Server::user(std::vector<std::string> &tokens, int fd)
{
	std::vector<std::string>::iterator tokens_it = tokens.begin();
	std::vector<Client>::iterator client_it = findClient(fd);

	handle_name(tokens);
	if (tokens.size() == 5 && (*(tokens_it + 4))[0] == ':' && (*(tokens_it + 4)).length() >= 2)
	{
		*(tokens_it + 4) = (*(tokens_it + 4)).substr(1, sizeof(*(tokens_it + 4)) - 1);
		if (client_it->getUname().empty() == false || client_it->getRname().empty() == false)
			sendReply(": you are already registered!",fd);
		else
		{
			client_it->setUname(*(tokens_it + 1));
			client_it->setRname(*(tokens_it + 4));
			client_it->is_registered = true;

			sendReply(RPL_WELCOME(client_it->getNick(), client_it->getUname(), this->hostname),fd);
			sendReply(RPL_YOURHOST(client_it->getNick(), this->hostname),fd);
			sendReply(RPL_CREATED(client_it->getNick(), this->date),fd);
		}
	}
	else
	{
		sendReply("Command form is: USER <username> <mode> <unused> :realname",fd);
	}
}

void Server::join(std::vector<std::string> &tokens, int fd)
{
	std::vector<std::string>::iterator tokens_it = tokens.begin();
	std::vector<Client>::iterator client_it = findClient(fd);
	if(tokens.size() == 2 && (*(tokens_it + 1))[0] == '#' && (*(tokens_it + 1)).length() >= 2)
	{
		std::vector<Channel>::iterator ch_it = findChannel(*(tokens_it + 1));
		if(ch_it != channels.end())
		{
			if(findClientInCh(ch_it,fd) == ch_it->clients_ch.end())
			{
				ch_it->addClient(findClient(fd));
				//sendToClientsInChannel(channel_it, RPL_JOIN(client_it->nickname, client_it->username, *(tokens_it + 1)));
				sendReply(RPL_JOIN(client_it->getNick(), client_it->getUname(), *(tokens_it + 1)),fd);
				/*if (channel_it->topic.length() == 0)
					sendReply(RPL_NOTOPIC(client_it->nickname, *(tokens_it + 1)));
				else
					sendReply(RPL_TOPIC(client_it->nickname, *(tokens_it + 1), channel_it->topic));
				if (channel_it->operator_array.size() > 0)
				{
					std::vector<client_t>::iterator it = channel_it->operator_array.begin();
					std::string users = it->nickname;
					it++;
					while (it != channel_it->operator_array.end())
					{
						users += (" " + it->nickname);
						it++;
					}
					sendReply(RPL_USRS(client_it->nickname, *(tokens_it + 1), users));
					sendReply(RPL_EONL(client_it->nickname, *(tokens_it + 1)));
				}
				else
					sendToClient(RPL_MODE(client_it->nickname, client_it->username, *(tokens_it + 1)));*/

			}
			else
				sendReply("Already joined to this channel", fd);
		}

		else
		{
			Channel new_ch((*(tokens_it + 1)), findClient(fd));
			channels.push_back(new_ch);
			sendReply(RPL_JOIN(client_it->getNick(), client_it->getUname(), *(tokens_it + 1)), fd);
			sendReply(RPL_MODE(client_it->getNick(), client_it->getUname(), *(tokens_it + 1)), fd);
			sendReply(RPL_NOTOPIC(client_it->getNick(), *(tokens_it + 1)), fd);
		}

	}
	else
		sendReply("Command form is: JOIN #channel",fd);
}