#include "Network.hpp"

using namespace std::literals::chrono_literals;

Network::Network(std::list< std::pair<std::string, int>> clientIp, int port, BlockChain *bc){
    m_clientsIp = clientIp;
    m_port = port;
    m_serv = new Server(port, &m_msgs, &m_mtx);
    m_bc = bc;
    
    m_serv->start();
    
    for (std::pair<std::string, int> host : m_clientsIp){
        m_serv->connectTo(host.first, host.second);
    }
    
    m_run.store(true, std::memory_order_relaxed);
    m_procThr = std::thread(&Network::process, this);
}

Network::~Network(){
    if (m_serv){
        delete m_serv;
    }
    
    m_run.store(false, std::memory_order_relaxed);
    
    m_procThr.join();
}

void Network::getBlocks(){
    std::list<int> clientsid = m_serv->getClientsId();
    
    int id = clientsid.front();
    
    std::list<std::array<uint32_t, 8>> lst;
    
    std::array<uint32_t, 8> hash = m_bc->getPastBlockHash();
    
    lst.push_back(hash);
    
    GetBlocksMsg *msg = new GetBlocksMsg(lst);
    
    m_serv->sendDataTo(id, msg);
    
    delete msg;
}

void Network::noFound(int clintId){
    
    NoFoundMsg *msg = new NoFoundMsg();
    
    m_serv->sendDataTo(clintId, msg);
    
    delete msg;
    
}

void Network::inv(std::array<uint32_t, 8> hash, int clientId){
    std::list<std::array<uint32_t, 8>> lst = m_bc->getHashesBefore(hash);
    
    InvMsg *msg = new InvMsg(InvTypes::iBlock, lst);
    
    m_serv->sendDataTo(clientId, msg);
    
    delete msg;
}

void Network::getData(std::list<std::array<uint32_t, 8>> hashes, int clientId){
    GetDataMsg *msg = new GetDataMsg(DataTypes::dBlock, hashes);
    m_serv->sendDataTo(clientId, msg);
    delete msg;
}

void Network::sblock(std::list<std::array<uint32_t, 8>> hashes, int clientId){
    for (std::array<uint32_t, 8> hash : hashes){
        Block *block = m_bc->getBlock(hash);
        if (block == nullptr){
            continue;
        }
        
        BlockMsg *msg = new BlockMsg(block);
            
        m_serv->sendDataTo(clientId, msg);
        
        delete block;
        delete msg;
    }
}


void Network::connectTo(std::string host, int ip){
    int res = m_serv->connectTo(host, ip);
    
    if (res == -1){
        return;
    }
    
    std::pair<std::string, int> conn(host, ip);
    
    m_clientsIp.push_back(conn);
}

void Network::process(){
    while (m_run.load(std::memory_order_relaxed)){
        if (m_msgs.size() == 0){
            std::this_thread::sleep_for(50ms);
            continue;
        }
        Message *msg = m_msgs.front();
        
        if (msg->getCommand() == MsgTypes::gBlocks){
            GetBlocksMsg *gdmsg = dynamic_cast<GetBlocksMsg *>(msg);
            std::list<std::array<uint32_t, 8>> hashes = gdmsg->getHashes();
            std::array<uint32_t, 8> hash = hashes.front();
            inv(hash, msg->getClientId());
        }
        
        else if (msg->getCommand() == MsgTypes::Inv){
            InvMsg *invmsg = dynamic_cast<InvMsg *>(msg);
            getData(invmsg->getHashes(), msg->getClientId());
        }
        
        else if (msg->getCommand() == MsgTypes::gData){
            GetDataMsg *gdatamsg = dynamic_cast<GetDataMsg *>(msg);
            sblock(gdatamsg->getHashes(), gdatamsg->getClientId());
        }
        
        else if (msg->getCommand() == MsgTypes::sBlock){
            BlockMsg *blockmsg = dynamic_cast<BlockMsg *>(msg);
            Block *block = blockmsg->getBlock();
            
            m_bc->putBlock(block);
            
            delete block;
        }
        
        else if (msg->getCommand() == MsgTypes::noFound){
        }
        
        m_mtx.lock();
        delete msg;
        m_msgs.pop_front();
        m_mtx.unlock();
        
        std::this_thread::sleep_for(50ms);
    }
}


