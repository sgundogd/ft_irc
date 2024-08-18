#include "Server.hpp"

int control(std::string port, std::string pass)
{
    int port_num;
    if (pass.length() <= 0 || port.length() <= 0)
		return (-1);
    else if (pass.find_first_of(" \r\n") != std::string::npos || port.find_first_of(" \r\n") != std::string::npos)
        return (-1);
   	
    for(size_t i = 0; i < port.length(); i++)
	{
		if (std::isdigit(port[i]) == false)
			return (-1);
	}

    try 
    {
        port_num = std::stoi(port);
    }

    catch(std::exception &e)
    {
        exit(1);
    } 

    if (port_num >= (1 << 16) || port_num < 1000)
        return (-1);
    return (port_num);
}
int main(int ac, char **av) 
{
    Server server;
    int port;
    std::string pass;
    if (ac != 3)
    {
        std::cerr << "INVALID ARGUMENT" << std::endl;
        return -1;
    }

    if(control(av[1],av[2]) == -1)
    {
        std::cerr << "INVALID ARGUMENT" << std::endl;
        return -1;       
    }
    else
        port = control(av[1],av[2]);
    pass = av[2];
    if(server.init(port, pass) == -1)
        exit(1);
    return(server.Start());
    return(0);
}