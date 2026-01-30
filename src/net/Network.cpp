#include "Network.hpp"

using namespace std::literals::chrono_literals;

Network::Network(int port, BlockChain& bc):
    m_bc(bc) {
    m_serv = std::make_unique<Server>(port, [this](uint8_t* data, size_t len, long clientId) {
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
    std::list<ClientID> clientIds = m_serv->getClientsId();

    if(clientIds.empty()) {
        return;
    }

    ClientID id = clientIds.front();
    std::list<Hash> lst;
    Hash hash = m_bc.getPastBlockHash();
    lst.push_back(hash);
    GetBlocksMsg msg = GetBlocksMsg(lst);

    ByteWriter byteWriter;
    msg.encode(byteWriter);
    const std::vector<std::byte> bytes = byteWriter.bytes();

    m_serv->sendDataTo(id, (uint8_t*) bytes.data(), bytes.size());
}

void Network::sendToMempool(const Transaction& tx) {
    std::list<ClientID> clientIds = m_serv->getClientsId();

    if(clientIds.empty()) {
        return;
    }

    ClientID id = clientIds.front();
    TxMsg msg = TxMsg(tx);

    ByteWriter byteWriter;
    msg.encode(byteWriter);
    const std::vector<std::byte> bytes = byteWriter.bytes();

    m_serv->sendDataTo(id, (uint8_t*) bytes.data(), bytes.size());
}

void Network::noFound(ClientID clintId) {
    NoFoundMsg msg = NoFoundMsg();

    ByteWriter byteWriter;
    msg.encode(byteWriter);
    const std::vector<std::byte> bytes = byteWriter.bytes();

    m_serv->sendDataTo(clintId, (uint8_t*) bytes.data(), bytes.size());
}

void Network::inv(const Hash& hash, ClientID clientId) {
    std::list<Hash> lst = m_bc.getHashesBefore(hash);
    InvMsg msg = InvMsg(InvTypes::iBlock, lst);

    ByteWriter byteWriter;
    msg.encode(byteWriter);
    const std::vector<std::byte> bytes = byteWriter.bytes();

    m_serv->sendDataTo(clientId, (uint8_t*) bytes.data(), bytes.size());
}

void Network::getData(const std::list<Hash>& hashes, ClientID clientId) {
    GetDataMsg msg = GetDataMsg(DataTypes::dBlock, hashes);

    ByteWriter byteWriter;
    msg.encode(byteWriter);
    const std::vector<std::byte> bytes = byteWriter.bytes();

    m_serv->sendDataTo(clientId, (uint8_t*) bytes.data(), bytes.size());
}

void Network::sblock(const std::list<Hash>& hashes, ClientID clientId) {
    for(const auto& hash: hashes) {
        std::optional<Block> block = m_bc.getBlock(hash);

        if(!block.has_value()) {
            continue;
        }

        BlockMsg msg = BlockMsg(*block);

        ByteWriter byteWriter;
        msg.encode(byteWriter);
        const std::vector<std::byte> bytes = byteWriter.bytes();

        m_serv->sendDataTo(clientId, (uint8_t*) bytes.data(), bytes.size());
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

void Network::messageHandler(uint8_t* buffer, size_t n, ClientID clientId) {
    MsgTypes type = static_cast<MsgTypes>(buffer[9]);

    ByteReader byteReader(as_bytes(buffer, n));

    switch(type) {
        case(MsgTypes::gBlocks): {
            auto getBlockMsg = std::make_unique<GetBlocksMsg>();
            getBlockMsg->decode(byteReader);

            const std::list<Hash>& hashes = getBlockMsg->getHashes();

            if(hashes.empty()) {
                break;
            }

            inv(hashes.front(), clientId);
            break;
        }
        case(MsgTypes::Inv): {
            auto invMsg = std::make_unique<GetBlocksMsg>();
            invMsg->decode(byteReader);
            getData(invMsg->getHashes(), clientId);
            break;
        }
        case(MsgTypes::gData): {
            auto getDataMsg = std::make_unique<GetDataMsg>();
            getDataMsg->decode(byteReader);
            sblock(getDataMsg->getHashes(), clientId);
            break;
        }
        case(MsgTypes::sBlock): {
            auto blockMsg = std::make_unique<BlockMsg>();
            blockMsg->decode(byteReader);
            m_bc.putBlock(blockMsg->getBlock());
            break;
        }
        case(MsgTypes::Tx): {
            auto txMsg = std::make_unique<TxMsg>();
            txMsg->decode(byteReader);

            m_mtx.lock();
            m_mempool.push_back(txMsg->getTransaction());
            m_mtx.unlock();
            break;
        }
        default:
            break;
    }
}

std::optional<Transaction> Network::getFromMempool() {
    std::optional<Transaction> tx;
    m_mtx.lock();

    if(m_mempool.size() != 0) {
        tx = m_mempool.front();
        m_mempool.pop_front();
    }

    m_mtx.unlock();
    return tx;
}
