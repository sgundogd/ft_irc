#include "Server.hpp"

void Server::nick(std::vector<std::string> &tokens, int fd)
{
	std::vector<std::string>::iterator tokens_it = tokens.begin();
	std::vector<Client>::iterator client_it = findClient(fd);
	
	if (tokens.size() == 2)
	{
		if (!isAlNumStr(*(tokens_it + 1)))
		{
			sendReply(ERR_NICK(*(tokens_it + 1)),fd);
		}
		else if (findClientNick(*(tokens_it + 1)) != clients.end())
		{
			sendReply(ERR_ALREADYREGISTERED(*(tokens_it + 1)),fd);
		}
		else
		{

            client_it->setNick(*(tokens_it + 1));
			//channel içindeki nicki de değiştir
			//fd atıp channel içindeki clientı bul ve chanel içinde adını değiştir
		}
	}
	else
	{
		sendReply("Command form is: NICK <nickname>", fd);
	}
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
			//client_it->is_registered = true;

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
