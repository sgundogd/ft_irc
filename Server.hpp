#ifndef __SERVER_H
# define __SERVER_H

# include <netinet/in.h>
# include <stdio.h>
# include <stdlib.h>
# include <string.h>
# include <sys/socket.h>
# include <unistd.h>
# include <fstream>
# include <arpa/inet.h>
# include <vector>
# include <iterator>
# include <sstream>
# include <string>
# include <iostream>
# include <fcntl.h>

# include "Client.hpp"
# include "Channel.hpp"
# include "errors.h"

# define BUFFER_SIZE 1024
# define QEUE 100

class Client;
class Channel;

class Server
{
    private:
        int                 serverfd;
        int                 maxfd;
        int                 c_id;
        int                 opt;
        struct  sockaddr_in servaddr;
        char                buff[BUFFER_SIZE];
        fd_set              read_set, write_set, current;
        socklen_t           len;
        std::vector<Client> clients;
        std::vector<Channel> channels;
        std::string hostname;
        std::string date;
        std::string passwd;

    public:
        Server();
        int init(int port, std::string pass);
        int Start();

        std::vector<Client>::iterator findClient(int fd);
        std::vector<Client>::iterator findClientInCh(std::vector<Channel>::iterator it, int fd);
        std::vector<Client>::iterator findClientNick(std::string &str);
        std::vector<Channel>::iterator findChannel(std::string str);
        void    sendToClis(int fd);
        void    sendToClisInCh(std::vector<Channel>::iterator it, std::string msg,int fd);
        void    parse_cl(int fd);
        bool    isAlNumStr(std::string str);
        void    handle_name(std::vector<std::string> &tokens);
        void    eraseClient(int fd);
        void    eraseClientFromCh(std::vector<Channel>::iterator it, int fd);

        void	pass(std::vector<std::string> &tokens, int fd);
		void	nick(std::vector<std::string> &tokens, int fd);
		void	user(std::vector<std::string> &tokens, int fd);
		void	privmsg(std::vector<std::string> &tokens, int fd);
		void	join(std::vector<std::string> &tokens, int fd);
		void	kick(std::vector<std::string> &token, int fd);
		void	quit(std::vector<std::string> &tokens, int fd);
		void	topic(std::vector<std::string> &tokens, int fd);
		void	notice(std::vector<std::string> &tokens, int fd);
		void	part(std::vector<std::string> &tokens, int fd);
		void	cap(std::vector<std::string> &tokens);
		void	sendReply(std::string msg, int fd);
		void	sendCl(std::string msg, int fd);
        ~Server();
};

#endif