#include "Network.hpp"

using namespace std::literals::chrono_literals;

Network::Network(int port, BlockChain &bc) : 
    m_bc(bc)
{
    m_serv = std::make_unique<Server>(port, 
        [this](uint8_t *data, size_t len, long clientId) {
            return this->messageHandler(data, len, clientId);
        });
    m_serv->start();

    for(std::pair<std::string, int> host: m_clientsIp) {
        m_serv->connectTo(host.first, host.second);
    }
}

Network::~Network() {
    m_serv->stop();
}

void Network::getBlocks() {
    std::list<int> clientsid = m_serv->getClientsId();
    int id = clientsid.front();
    std::list<std::array<uint8_t, 32>> lst;
    std::array<uint8_t, 32> hash = m_bc.getPastBlockHash();
    lst.push_back(hash);
    GetBlocksMsg msg = GetBlocksMsg(lst);

    auto bytes = msg.toByte();

    m_serv->sendDataTo(id, bytes.data(), bytes.size());
}

void Network::sendToMempool(std::unique_ptr<Transaction> tx) {
    std::list<int> clientsid = m_serv->getClientsId();
    int id = clientsid.front();
    TxMsg msg = TxMsg(tx);

    auto bytes = msg.toByte();

    m_serv->sendDataTo(id, bytes.data(), bytes.size());
}

void Network::noFound(int clintId) {
    NoFoundMsg msg = NoFoundMsg();

    auto bytes = msg.toByte();

    m_serv->sendDataTo(clintId, bytes.data(), bytes.size());
}

void Network::inv(std::array<uint8_t, 32> hash, int clientId) {
    std::list<std::array<uint8_t, 32>> lst = m_bc.getHashesBefore(hash);
    InvMsg msg = InvMsg(InvTypes::iBlock, lst);

    auto bytes = msg.toByte();

    m_serv->sendDataTo(clientId, bytes.data(), bytes.size());
}

void Network::getData(std::list<std::array<uint8_t, 32>> hashes, int clientId) {
    GetDataMsg msg = GetDataMsg(DataTypes::dBlock, hashes);

    auto bytes = msg.toByte();

    m_serv->sendDataTo(clientId, bytes.data(), bytes.size());
}

void Network::sblock(std::list<std::array<uint8_t, 32>> hashes, int clientId) {
    for(std::array<uint8_t, 32> hash: hashes) {
        auto block = m_bc.getBlock(hash);

        if(!block) {
            continue;
        }

        BlockMsg msg = BlockMsg(block);

        auto bytes = msg.toByte();

        m_serv->sendDataTo(clientId, bytes.data(), bytes.size());
        std::this_thread::sleep_for(50ms);
    }
}

void Network::connectTo(const std::string& host, int ip) {
    int res = m_serv->connectTo(host, ip);

    if(res == -1) {
        return;
    }

    std::pair<std::string, int> conn(host, ip);
    m_clientsIp.push_back(conn);
}

void Network::messageHandler(uint8_t *buffer, size_t n, long clientId) {
    uint8_t type = buffer[9];

    switch (type) {
    case (MsgTypes::gBlocks): {
        auto getBlockMsg = std::make_unique<GetBlocksMsg>();
        getBlockMsg->parse(buffer, n);

        std::list<std::array<uint8_t, 32>> hashes = getBlockMsg->getHashes();
        std::array<uint8_t, 32> hash = hashes.front();
        inv(hash, clientId);
        break;

    } case (MsgTypes::Inv): {
        auto invMsg = std::make_unique<GetBlocksMsg>();
        invMsg->parse(buffer, n);
        getData(invMsg->getHashes(), clientId);
        break;
    
    } case(MsgTypes::gData) : {
        auto getDataMsg = std::make_unique<GetDataMsg>();
        getDataMsg->parse(buffer, n);
        sblock(getDataMsg->getHashes(), clientId);
        break;

    } case(MsgTypes::sBlock) : {
        auto blockMsg = std::make_unique<BlockMsg>();
        blockMsg->parse(buffer, n);
        auto block = std::unique_ptr<Block>(blockMsg->getBlock());
        m_bc.putBlock(block);
        break;

    } case (MsgTypes::Tx) : {
        auto txMsg = std::make_unique<TxMsg>();
        txMsg->parse(buffer, n);

        m_mtx.lock();
        m_mempool.push_back(std::make_unique<Transaction>(txMsg->getTransaction()));
        m_mtx.unlock();
        break;
    
    } default:
        break;
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
