#ifndef __CLIENT_HPP
# define __CLIENT_HPP

# include <iostream>
# include <fcntl.h>
# include <unistd.h>

class Client
{
    private:
        int         fd;
        int         id;
        std::string nick;
        std::string rname;
        std::string uname;
    public:
        Client(int c_fd, int c_id);
        ~Client();

        void setNick(std::string c_nick);
        void setUname(std::string c_uname);
        void setRname(std::string c_rname);
        bool isempty_u();
        bool isempty_n();
        std::string getNick(void) const;
        std::string getUname(void) const;
        std::string getRname(void) const;
        int getId(void) const;
        int getFd(void) const;
};

#endif
