#include "Server.hpp"

Server::Server() {}

int Server::init(int port, std::string pass)
{
    opt = 1;
    char hostname[1024];
    gethostname(hostname,sizeof(hostname));
    this->hostname = hostname;
    this->passwd = pass;
    c_id = 0;
    if ((serverfd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
        return (-1);
    if (setsockopt(serverfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) != 0)
        return (-1);
    if (fcntl(serverfd, F_SETFL, O_NONBLOCK) < 0)
        return -1;
    maxfd = serverfd;
    memset(&servaddr, 0, sizeof(servaddr));
    (servaddr).sin_port = htons(port);
    (servaddr).sin_addr.s_addr = INADDR_ANY;
    (servaddr).sin_family = AF_INET;
    if ((bind(serverfd, (const sockaddr *)(&servaddr), sizeof(servaddr))) == -1)
        return (-1);
    return (0);
}

int Server::Start()
{
    FD_ZERO(&current);
    FD_SET(serverfd, &current);
    if (listen(serverfd, QEUE) == -1)
        return (-1);
    memset(buff, 0, sizeof(buff));
    len = sizeof(servaddr);
    while (1)
    {
        write_set = read_set = current;

        if (select(maxfd + 1, &read_set, &write_set, 0, 0) < 0)
            return (-1);
        for (int fd = 0; fd <= maxfd; fd++)
        {
            if (FD_ISSET(fd, &read_set))
            {
                int clientfd;
                int ret;
                if (fd == serverfd)
                {
                    if ((clientfd = accept(serverfd, (struct sockaddr *)&servaddr, &len)) < 0)
                        return (-1);
                    c_id++;
                    Client client(clientfd, c_id);
                    clients.push_back(client);
                    FD_SET(clientfd, &current);
                    if (maxfd < clientfd)
                        maxfd = clientfd;
                }
                else
                {
                    ret = recv(fd, buff, sizeof(buff), 0);
                    if (ret <= 0)
                    {
                        if (ret == 0)
                            std::cout << "Client " << findClient(fd)->getId() << " left the server" << std::endl;
                        else
                            std::cerr << "recv error\n";
                        // buraya client kaldırma için ayrı fonksiyon eklenmiş
                        FD_CLR(fd, &current);
                        close(fd);
                        clients.erase(findClient(fd));
                    }
                    else if (ret == 1 && *buff == '\n')
                        continue;
                    else
                    {
                        buff[ret-1] = '\0';
                        parse_cl(fd);
                        // buraya cemal ekstra koşullar eklemiş bir tek /n var gibi vb onlara bak
                        sendToClis(fd);
                    }
                    memset(buff, 0, sizeof(buff));
                }
            }
        }
    }
}

void Server::parse_cl(int fd)
{
    std::istringstream iss(buff);
    std::string line;
	std::vector<std::string> lines;
    std::vector<Client>::iterator client_it = findClient(fd);
	while (std::getline(iss, line, '\n'))
	{
		if (line.length() == 0 || line == "\n")
			continue ;
		else if (line[line.length() - 1] == '\r')
			lines.push_back(line.substr(0, line.length() - 1));
		else
    		lines.push_back(line);
    }
    std::vector<std::string>::iterator lines_it = lines.begin();
    while (lines_it != lines.end())
	{
		std::string token;
		std::vector<std::string> tokens;
		std::istringstream isslines(*lines_it);

		while (std::getline(isslines, token, ' '))
		{
			if (token.length() == 0 || token == "\n")
				continue ;
    		tokens.push_back(token);
    	}
	
		std::vector<std::string>::iterator tokens_it = tokens.begin();
		if (*tokens_it == "PASS")
			pass(tokens, fd);
		else if (*tokens_it == "NICK")
			nick(tokens, fd);
		else if (*tokens_it == "USER")
			user(tokens, fd);
		else if (*tokens_it == "CAP")
			cap(tokens);
        else if((*tokens_it == "JOIN" || *tokens_it == "PRIVMSG" || *tokens_it == "KICK" 
        || *tokens_it == "QUIT") && !client_it->is_auth)
            sendReply("PLEASE LOGIN FIRST\n", fd);
        else if (*tokens_it == "JOIN" && client_it->is_auth)
			join(tokens, fd);
		else if (*tokens_it == "PRIVMSG" && client_it->is_auth)
			privmsg(tokens, fd);
		else if (*tokens_it == "KICK" && client_it->is_auth)
			kick(tokens);
		else if (*tokens_it == "QUIT" && client_it->is_auth)
			quit(tokens, fd);
		//else if (*tokens_it == "NOTICE")
		//	noticeCommand(tokens);
		//else if (*tokens_it == "PART")
		//	partCommand(tokens);
		//else if (*tokens_it == "TOPIC")
		//	topicCommand(tokens);
		lines_it++;
	}
}

std::vector<Client>::iterator Server::findClient(int fd)
{
    std::vector<Client>::iterator it = clients.begin();
    while (it != clients.end())
    {
        if (it->getFd() == fd)
            return (it);
        it++;
    }
    return (clients.end());
}

std::vector<Client>::iterator Server::findClientNick(std::string &str)
{
    std::vector<Client>::iterator it = clients.begin();
	while (it != clients.end())
	{
		if ((*it).getNick() == str)
			return it;
        it++;
	}
	return clients.end();
}
std::vector<Client>::iterator Server::findClientInCh(std::vector<Channel>::iterator it, int fd)
{
    
    std::vector<Client>::iterator client_it = it->clients_ch.begin();
    while (client_it != it->clients_ch.end())
    {
        if (client_it->getFd() == fd)
            return (client_it);
        client_it++;
    }
    return (client_it);
}

std::vector<Channel>::iterator Server::findChannel(std::string str)
{
   std::vector<Channel>::iterator channel_it = channels.begin();
   while (channel_it != channels.end())
   {
    if(channel_it->getName() == str)
        return(channel_it);
    channel_it++;
   }
    return (channel_it);
}

void Server::sendToClis(int fd)
{
    std::string meta = std::to_string(findClient(fd)->getId()) + ": ";
    for (int i = 0; i <= maxfd; i++)
    {
        if (i != fd && FD_ISSET(i, &write_set))
        {
            send(i, meta.c_str(), sizeof(meta), 0);
            send(i, buff, sizeof(buff), 0);
        }
    }
}

void Server::sendToClisInCh(std::vector<Channel>::iterator it, std::string msg,int fd)
{
    std::vector<Client>::iterator client_it = it->clients_ch.begin();
    while (client_it != it->clients_ch.end())
    {
        if (client_it->getFd() != fd)
            send(client_it->getFd(), msg.c_str(), sizeof(msg), 0);
        client_it++;
    }
}

void Server::sendReply(std::string msg, int fd)
{
    for (int i = 0; i <= maxfd; i++)
    {
        if (i == fd)
        {

            send(i, msg.c_str(), sizeof(msg), 0);
        }
    }
    //std::string reply = ":" + this->hostname + msg + "\r\n";
    //send(fd, reply.c_str(), reply.size(), 0);
}

bool Server::isAlNumStr(std::string str)
{
	for (size_t i = 0; i < str.length(); i++)
	{
		if (!std::isalnum(str[i]))
			return false;
	}
	return true;
}
void Server::handle_name(std::vector<std::string> &tokens)
{
    std::vector<std::string>::iterator token = tokens.begin();
    while (token != tokens.end())
    {
        if((*token)[0] == ':')
            break;
        token++;
    }
    if(token == tokens.end())
        return;
    while (token+1 != tokens.end())
    {
        (*token).append(" ");
        (*token).append((*(token + 1)));
        tokens.erase(token+1);
    }  
}

Server::~Server() {}