#ifndef __CHANNEL_HPP
# define __CHANNEL_HPP

# include "Client.hpp"
# include <vector>

class Client;

class Channel
{
    private:
    std::string channel_name;
    std::string topic; // Kanalın konusu bunu kullanıcaz mı bilmiyorum  
    std::vector<Client> clients_ch;
    std::vector<Client> operators;

    public:
    //client ekle
    //client çıkar
    //konu değiştir
    //std::vector<Client>::iterator find_client_ch(int fd);
    //std::string getName() const { return name; }
    //std::string getTopic() const { return topic; }
    //void setTopic(const std::string& newTopic) { topic = newTopic; }

    //void addOperator(Client* client);
    //void removeOperator(Client* client);
    //bool isOperator(Client* client) const;
    Channel();
    ~Channel();
};

#endif