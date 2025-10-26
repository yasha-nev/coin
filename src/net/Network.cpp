#include "Network.hpp"

#include <memory>

using namespace std::literals::chrono_literals;

Network::Network(std::list<std::pair<std::string, int>> clientIp, int port, BlockChain* bc) {
    m_clientsIp = clientIp;
    m_port = port;
    m_serv = std::make_unique<Server>(port, &m_msgs, &m_mtx);
    m_bc = bc;
    m_serv->start();

    for(std::pair<std::string, int> host: m_clientsIp) {
        m_serv->connectTo(host.first, host.second);
    }

    m_run.store(true, std::memory_order_relaxed);
    m_procThr = std::thread(&Network::process, this);
}

Network::~Network() {
    m_run.store(false, std::memory_order_relaxed);
    m_procThr.join();
}

void Network::getBlocks() {
    std::list<int> clientsid = m_serv->getClientsId();
    int id = clientsid.front();
    std::list<std::array<uint32_t, 8>> lst;
    std::array<uint32_t, 8> hash = m_bc->getPastBlockHash();
    lst.push_back(hash);
    GetBlocksMsg msg = GetBlocksMsg(lst);
    m_serv->sendDataTo(id, &msg);
}

void Network::sendToMempool(std::unique_ptr<Transaction> tx) {
    std::list<int> clientsid = m_serv->getClientsId();
    int id = clientsid.front();
    TxMsg msg = TxMsg(tx);
    m_serv->sendDataTo(id, &msg);
}

void Network::noFound(int clintId) {
    NoFoundMsg msg = NoFoundMsg();
    m_serv->sendDataTo(clintId, &msg);
}

void Network::inv(std::array<uint32_t, 8> hash, int clientId) {
    std::list<std::array<uint32_t, 8>> lst = m_bc->getHashesBefore(hash);
    InvMsg msg = InvMsg(InvTypes::iBlock, lst);
    m_serv->sendDataTo(clientId, &msg);
}

void Network::getData(std::list<std::array<uint32_t, 8>> hashes, int clientId) {
    GetDataMsg msg = GetDataMsg(DataTypes::dBlock, hashes);
    m_serv->sendDataTo(clientId, &msg);
}

void Network::sblock(std::list<std::array<uint32_t, 8>> hashes, int clientId) {
    for(std::array<uint32_t, 8> hash: hashes) {
        auto block = m_bc->getBlock(hash);

        if(!block) {
            continue;
        }

        BlockMsg msg = BlockMsg(block);
        m_serv->sendDataTo(clientId, &msg);
        std::this_thread::sleep_for(50ms);
    }
}

void Network::connectTo(std::string host, int ip) {
    int res = m_serv->connectTo(host, ip);

    if(res == -1) {
        return;
    }

    std::pair<std::string, int> conn(host, ip);
    m_clientsIp.push_back(conn);
}

void Network::process() {
    while(m_run.load(std::memory_order_relaxed)) {
        if(m_msgs.size() == 0) {
            std::this_thread::sleep_for(50ms);
            continue;
        }

        m_mtx.lock();
        auto msg = std::move(m_msgs.front());
        m_msgs.pop_front();
        m_mtx.unlock();

        if(msg->getCommand() == MsgTypes::gBlocks) {
            GetBlocksMsg* gdmsg = dynamic_cast<GetBlocksMsg*>(msg.get());
            std::list<std::array<uint32_t, 8>> hashes = gdmsg->getHashes();
            std::array<uint32_t, 8> hash = hashes.front();
            inv(hash, msg->getClientId());
        }

        else if(msg->getCommand() == MsgTypes::Inv) {
            InvMsg* invmsg = dynamic_cast<InvMsg*>(msg.get());
            getData(invmsg->getHashes(), msg->getClientId());
        }

        else if(msg->getCommand() == MsgTypes::gData) {
            GetDataMsg* gdatamsg = dynamic_cast<GetDataMsg*>(msg.get());
            sblock(gdatamsg->getHashes(), gdatamsg->getClientId());
        }

        else if(msg->getCommand() == MsgTypes::sBlock) {
            BlockMsg* blockmsg = dynamic_cast<BlockMsg*>(msg.get());
            auto block = std::unique_ptr<Block>(new Block(blockmsg->getBlock()));
            m_bc->putBlock(block);
        }

        else if(msg->getCommand() == MsgTypes::Tx) {
            TxMsg* txmsg = dynamic_cast<TxMsg*>(msg.get());
            m_mtx.lock();
            m_mempool.push_back(std::make_unique<Transaction>(txmsg->getTransaction()));
            m_mtx.unlock();
        }

        else if(msg->getCommand() == MsgTypes::noFound) {
        }
        std::this_thread::sleep_for(50ms);
    }
}

std::unique_ptr<Transaction> Network::getFromMempool() {
    std::unique_ptr<Transaction> tx;
    m_mtx.lock();

    if(m_mempool.size() != 0) {
        tx = std::move(m_mempool.front());
        m_mempool.pop_front();
    }

    m_mtx.unlock();
    return tx;
}
