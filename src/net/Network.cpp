#include "net/Network.hpp"

using namespace std::literals::chrono_literals;

Network::Network(int port, BlockChain& bc):
    m_bc(bc) {
    m_serv = std::make_unique<Server>(
        port, [this](std::span<const std::byte> buffer, long clientId) {
            return this->messageHandler(buffer, clientId);
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

    m_serv->sendDataTo(id, byteWriter.bytes());
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

    m_serv->sendDataTo(id, byteWriter.bytes());
}

void Network::noFound(ClientID clintId) {
    NoFoundMsg msg = NoFoundMsg();

    ByteWriter byteWriter;
    msg.encode(byteWriter);

    m_serv->sendDataTo(clintId, byteWriter.bytes());
}

void Network::inv(const Hash& hash, ClientID clientId) {
    std::list<Hash> lst = m_bc.getHashesBefore(hash);
    InvMsg msg = InvMsg(InvTypes::iBlock, lst);

    ByteWriter byteWriter;
    msg.encode(byteWriter);

    m_serv->sendDataTo(clientId, byteWriter.bytes());
}

void Network::getData(const std::list<Hash>& hashes, ClientID clientId) {
    GetDataMsg msg = GetDataMsg(DataTypes::dBlock, hashes);

    ByteWriter byteWriter;
    msg.encode(byteWriter);

    m_serv->sendDataTo(clientId, byteWriter.bytes());
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

        m_serv->sendDataTo(clientId, byteWriter.bytes());
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

void Network::messageHandler(std::span<const std::byte> buffer, ClientID clientId) {

    if(buffer.size() < sizeof(MessageHeader)) {
        return;
    }

    MessageHeader header {};
    std::memcpy(&header, buffer.data(), sizeof(MessageHeader));

    if(buffer.size() < sizeof(MessageHeader) + header.payloadSize) {
        return;
    }

    ByteReader byteReader(buffer);

    MsgType type = header.command;

    switch(type) {
        case(MsgType::gBlocks): {
            GetBlocksMsg getBlockMsg;
            getBlockMsg.decode(byteReader);

            const std::list<Hash>& hashes = getBlockMsg.getHashes();

            if(hashes.empty()) {
                break;
            }

            inv(hashes.front(), clientId);
            break;
        }
        case(MsgType::Inv): {
            InvMsg invMsg;
            invMsg.decode(byteReader);
            getData(invMsg.getHashes(), clientId);
            break;
        }
        case(MsgType::gData): {
            GetDataMsg getDataMsg;
            getDataMsg.decode(byteReader);
            sblock(getDataMsg.getHashes(), clientId);
            break;
        }
        case(MsgType::sBlock): {
            BlockMsg blockMsg;
            blockMsg.decode(byteReader);
            m_bc.putBlock(blockMsg.getBlock());
            break;
        }
        case(MsgType::Tx): {
            TxMsg txMsg;
            txMsg.decode(byteReader);

            std::lock_guard<std::mutex> lock(m_mtx);
            m_mempool.push_back(txMsg.getTransaction());
            break;
        }
        case(MsgType::noFound): {
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
