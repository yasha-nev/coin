
#ifndef Network_hpp
#define Network_hpp

#include "Server.hpp"
#include "Message.hpp"
#include "BlockChain.hpp"
#include <utility>

class Network{
public:
    
    Network(std::list< std::pair<std::string, int>> clientIp, int port, BlockChain *bc);
    ~Network();
    
    void getBlocks();
    
    void connectTo(std::string host, int ip);
    
private:
    
    void process();
    
    void noFound(int clientId);
    
    void inv(std::array<uint32_t, 8> hash, int clientId);
    
    void getData(std::list<std::array<uint32_t, 8>> hashes, int clientId);
    
    void sblock(std::list<std::array<uint32_t, 8>> hashes, int clientId);
    
    Server *m_serv;
    
    BlockChain *m_bc;
    
    std::list<Message *> m_msgs;

    std::list <std::pair<std::string, int>> m_clientsIp;
    
    std::atomic<bool> m_run;
    std::mutex m_mtx;
    std::thread m_procThr;
    int m_port;
};

#endif

