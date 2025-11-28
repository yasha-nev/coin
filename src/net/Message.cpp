#include "Message.hpp"

#define STARTSIZE 9

Message::~Message() {
}

uint8_t Message::getCommand() {
    return m_comm;
}

void Message::setClientId(int id) {
    m_clientId = id;
}

int Message::getClientId() {
    return m_clientId;
}

GetBlocksMsg::GetBlocksMsg() {
    m_comm = MsgTypes::gBlocks;
    m_ver = 0;
}

GetBlocksMsg::GetBlocksMsg(const std::list<std::array<uint8_t, 32>> &hashes) {
    m_comm = MsgTypes::gBlocks;
    m_ver = 0;
    m_hashes = hashes;
}

std::list<std::array<uint8_t, 32>> GetBlocksMsg::getHashes() {
    return m_hashes;
}

void GetBlocksMsg::parse(uint8_t* data, size_t size) {
    size_t headerSize = sizeof(char) * STARTSIZE + sizeof(uint8_t) + sizeof(size_t) +
        sizeof(uint8_t) * 32;
    size_t count = 0;
    size_t payload_size;

    memcpy(&payload_size, data + sizeof(char) * STARTSIZE + sizeof(uint8_t), sizeof(size_t));
    size_t ptr = headerSize;
    memcpy(&m_ver, data + ptr, sizeof(uint8_t));
    ptr += sizeof(uint8_t);
    memcpy(&count, data + ptr, sizeof(size_t));
    ptr += sizeof(size_t);

    for(int i = 0; i < count - 1; i++) {
        std::array<uint8_t, 32> hash;
        for(int j = 0; j < 32; j++) {
            uint32_t h;
            memcpy(&h, data + ptr + j * sizeof(uint8_t), sizeof(uint8_t));
            hash[j] = h;
        }

        m_hashes.push_back(hash);
        ptr += sizeof(uint32_t) * 8;
    }
}

std::vector<uint8_t> GetBlocksMsg::toByte() const {
    size_t count = m_hashes.size() + 1;
    size_t payloadSize = sizeof(uint8_t) + sizeof(size_t) +
        sizeof(uint32_t) * 8 * (m_hashes.size() + 1);
    size_t headerSize = sizeof(char) * STARTSIZE + sizeof(uint8_t) + sizeof(size_t) +
        sizeof(uint32_t) * 8;
    
    size_t size = headerSize + payloadSize;

    std::vector<uint8_t> byteArray(size);
    uint8_t* msg = byteArray.data();
    size_t ptr = 0;
    

    // create header

    memcpy(msg + ptr, STARTSTR, STARTSIZE);
    ptr += STARTSIZE;
    memcpy(msg + ptr, &m_comm, sizeof(uint8_t));
    ptr += sizeof(uint8_t);
    memcpy(msg + ptr, &payloadSize, sizeof(size_t));
    ptr += sizeof(size_t);

    ptr = headerSize;

    // create payload

    memcpy(msg + ptr, &m_ver, sizeof(uint8_t));
    ptr += sizeof(uint8_t);
    memcpy(msg + ptr, &count, sizeof(size_t));
    ptr += sizeof(size_t);

    for(std::array<uint8_t, 32> hash: m_hashes) {
        for(int j = 0; j < 32; j++) {
            uint32_t h = hash[j];
            memcpy(msg + ptr + sizeof(uint8_t) * j, &h, sizeof(uint8_t));
        }

        ptr += sizeof(uint32_t) * 8;
    }

    uint8_t zerohash[32] = {0};
    memcpy(msg + ptr, zerohash, sizeof(uint8_t) * 32);
    ptr += sizeof(uint8_t) * 32;

    // create checksum

    CryptoppImpl cryptor;
    std::array<uint8_t, 32> checksum = cryptor.sha256Hash(
        std::string(reinterpret_cast<char*>(msg + headerSize), payloadSize));
    ptr = sizeof(char) * STARTSIZE + sizeof(uint8_t) + sizeof(size_t);
    memcpy(msg + ptr, checksum.data(), sizeof(uint8_t) * 32);

    return byteArray;
}

void GetBlocksMsg::print() const {
    CryptoppImpl cryptor;
    std::cout << "GetBlocksMsg\n";
    for(auto& itr: m_hashes) {
        std::cout << cryptor.sha256HashToString(itr) << "\n";
    }
    std::cout << std::endl;
}

InvMsg::InvMsg() {
    m_comm = MsgTypes::Inv;
    m_type = 0;
}

InvMsg::InvMsg(InvTypes type, std::list<std::array<uint8_t, 32>> hashes) {
    m_comm = MsgTypes::Inv;
    m_hashes = hashes;
    m_type = static_cast<uint8_t>(type);
}

std::list<std::array<uint8_t, 32>> InvMsg::getHashes() {
    return m_hashes;
}

void InvMsg::parse(uint8_t* data, size_t size) {
    size_t headerSize = sizeof(char) * STARTSIZE + sizeof(uint8_t) + sizeof(size_t) +
        sizeof(uint32_t) * 8;
    size_t count = 0;
    size_t payload_size;
    memcpy(&payload_size, data + sizeof(char) * STARTSIZE + sizeof(uint8_t), sizeof(size_t));

    size_t ptr = headerSize;
    memcpy(&count, data + ptr, sizeof(size_t));
    ptr += sizeof(size_t);

    for(int i = 0; i < count - 1; i++) {
        memcpy(&m_type, data + ptr, sizeof(uint8_t));
        ptr += sizeof(uint8_t);
        std::array<uint8_t, 32> hash;
        for(int j = 0; j < 32; j++) {
            uint32_t h;
            memcpy(&h, data + ptr + j * sizeof(uint8_t), sizeof(uint8_t));
            hash[j] = h;
        }

        m_hashes.push_back(hash);
        ptr += sizeof(uint32_t) * 8;
    }
}

std::vector<uint8_t> InvMsg::toByte() const {
    size_t count = m_hashes.size() + 1;
    size_t payloadSize = sizeof(size_t) + sizeof(uint8_t) * (m_hashes.size()) +
        sizeof(uint32_t) * 8 * (m_hashes.size());
    size_t headerSize = sizeof(char) * STARTSIZE + sizeof(uint8_t) + sizeof(size_t) +
        sizeof(uint32_t) * 8;
    size_t size = headerSize + payloadSize;

    std::vector<uint8_t> byteArray(size);
    uint8_t* msg = byteArray.data();
    size_t ptr = 0;

    // create header

    memcpy(msg + ptr, STARTSTR, STARTSIZE);
    ptr += STARTSIZE;
    memcpy(msg + ptr, &m_comm, sizeof(uint8_t));
    ptr += sizeof(uint8_t);
    memcpy(msg + ptr, &size, sizeof(size_t));
    ptr += sizeof(size_t);

    // create Payload
    ptr = headerSize;

    memcpy(msg + ptr, &count, sizeof(size_t));
    ptr += sizeof(size_t);
    for(std::array<uint8_t, 32> hash: m_hashes) {
        memcpy(msg + ptr, &m_type, sizeof(uint8_t));
        ptr += sizeof(uint8_t);

        for(int j = 0; j < 32; j++) {
            uint32_t h = hash[j];
            memcpy(msg + ptr + sizeof(uint8_t) * j, &h, sizeof(uint8_t));
        }

        ptr += sizeof(uint32_t) * 8;
    }

    // create Checksum

    CryptoppImpl cryptor;
    std::array<uint8_t, 32> checksum = cryptor.sha256Hash(
        std::string(reinterpret_cast<char*>(msg + headerSize), payloadSize));
    ptr = sizeof(char) * STARTSIZE + sizeof(uint8_t) + sizeof(size_t);
    memcpy(msg + ptr, checksum.data(), sizeof(uint8_t) * 32);

    return byteArray;
}

void InvMsg::print() const {
    CryptoppImpl cryptor;
    std::cout << "InvMsg\n";
    std::cout << "Type: " << m_type << "\n";
    for(auto& itr: m_hashes) {
        std::cout << cryptor.sha256HashToString(itr) << "\n";
    }
    std::cout << std::endl;
}

GetDataMsg::GetDataMsg() {
    m_comm = MsgTypes::gData;
    m_type = 0;
}

GetDataMsg::GetDataMsg(DataTypes type, std::list<std::array<uint8_t, 32>> hashes) {
    m_comm = MsgTypes::gData;
    m_hashes = hashes;
    m_type = static_cast<uint8_t>(type);
}

std::list<std::array<uint8_t, 32>> GetDataMsg::getHashes() {
    return m_hashes;
}

void GetDataMsg::parse(uint8_t* data, size_t size) {
    size_t headerSize = sizeof(char) * STARTSIZE + sizeof(uint8_t) + sizeof(size_t) +
        sizeof(uint32_t) * 8;
    size_t count = 0;
    size_t payload_size;
    memcpy(&payload_size, data + sizeof(char) * STARTSIZE + sizeof(uint8_t), sizeof(size_t));

    size_t ptr = headerSize;
    memcpy(&count, data + ptr, sizeof(size_t));
    ptr += sizeof(size_t);

    for(int i = 0; i < count - 1; i++) {
        memcpy(&m_type, data + ptr, sizeof(uint8_t));
        ptr += sizeof(uint8_t);
        std::array<uint8_t, 32> hash;
        for(int j = 0; j < 32; j++) {
            uint32_t h;
            memcpy(&h, data + ptr + j * sizeof(uint8_t), sizeof(uint8_t));
            hash[j] = h;
        }

        m_hashes.push_back(hash);
        ptr += sizeof(uint32_t) * 8;
    }
}

std::vector<uint8_t> GetDataMsg::toByte() const {
    size_t count = m_hashes.size() + 1;
    size_t payloadSize = sizeof(size_t) + sizeof(uint8_t) * (m_hashes.size()) +
        sizeof(uint32_t) * 8 * (m_hashes.size());
    size_t headerSize = sizeof(char) * STARTSIZE + sizeof(uint8_t) + sizeof(size_t) +
        sizeof(uint32_t) * 8;
    size_t size = headerSize + payloadSize;

    std::vector<uint8_t> byteArray(size);
    uint8_t* msg = byteArray.data();
    size_t ptr = 0;

    // create header

    memcpy(msg + ptr, STARTSTR, STARTSIZE);
    ptr += STARTSIZE;
    memcpy(msg + ptr, &m_comm, sizeof(uint8_t));
    ptr += sizeof(uint8_t);
    memcpy(msg + ptr, &size, sizeof(size_t));
    ptr += sizeof(size_t);

    // create Payload
    ptr = headerSize;

    memcpy(msg + ptr, &count, sizeof(size_t));
    ptr += sizeof(size_t);
    for(std::array<uint8_t, 32> hash: m_hashes) {
        memcpy(msg + ptr, &m_type, sizeof(uint8_t));
        ptr += sizeof(uint8_t);

        for(int j = 0; j < 32; j++) {
            uint32_t h = hash[j];
            memcpy(msg + ptr + sizeof(uint8_t) * j, &h, sizeof(uint8_t));
        }

        ptr += sizeof(uint32_t) * 8;
    }

    // create Checksum

    CryptoppImpl cryptor;
    std::array<uint8_t, 32> checksum = cryptor.sha256Hash(
        std::string(reinterpret_cast<char*>(msg + headerSize), payloadSize));
    ptr = sizeof(char) * STARTSIZE + sizeof(uint8_t) + sizeof(size_t);
    memcpy(msg + ptr, checksum.data(), sizeof(uint8_t) * 32);

    return byteArray;
}

void GetDataMsg::print() const {
    CryptoppImpl cryptor;
    std::cout << "GetDataMsg\n";
    std::cout << "Type: " << m_type << "\n";
    for(auto& itr: m_hashes) {
        std::cout << cryptor.sha256HashToString(itr) << "\n";
    }
    std::cout << std::endl;
}

BlockMsg::BlockMsg() {
    m_comm = MsgTypes::sBlock;
    m_block = nullptr;
}

BlockMsg::BlockMsg(std::unique_ptr<Block>& block) {
    m_comm = MsgTypes::sBlock;
    m_block = std::make_unique<Block>(block.get());
}

Block* BlockMsg::getBlock() {
    return m_block.get();
}

void BlockMsg::parse(uint8_t* data, size_t size) {
    size_t headerSize = sizeof(char) * STARTSIZE + sizeof(uint8_t) + sizeof(size_t) +
        sizeof(uint32_t) * 8;
    size_t payload_size;
    memcpy(&payload_size, data + sizeof(char) * STARTSIZE + sizeof(uint8_t), sizeof(size_t));

    size_t ptr = headerSize;

    m_block.release();
    m_block.reset(new Block());
    m_block->decode(data + ptr);
}

std::vector<uint8_t> BlockMsg::toByte() const {
    size_t payloadSize = m_block->size();
    size_t headerSize = sizeof(char) * STARTSIZE + sizeof(uint8_t) + sizeof(size_t) +
        sizeof(uint32_t) * 8;
    size_t size = headerSize + payloadSize;

    std::vector<uint8_t> byteArray(size);
    uint8_t* msg = byteArray.data();
    size_t ptr = 0;

    // create header

    memcpy(msg + ptr, STARTSTR, STARTSIZE);
    ptr += STARTSIZE;
    memcpy(msg + ptr, &m_comm, sizeof(uint8_t));
    ptr += sizeof(uint8_t);
    memcpy(msg + ptr, &size, sizeof(size_t));
    ptr += sizeof(size_t);

    // create Payload
    ptr = headerSize;
    std::vector<uint8_t> encBlock(m_block->size());
    m_block->encode(encBlock.data());
    memcpy(msg + ptr, encBlock.data(), m_block->size());
    ptr += m_block->size();

    // create Checksum

    CryptoppImpl cryptor;
    std::array<uint8_t, 32> checksum = cryptor.sha256Hash(
    std::string(reinterpret_cast<char*>(msg + headerSize), payloadSize));
    ptr = sizeof(char) * STARTSIZE + sizeof(uint8_t) + sizeof(size_t);
    memcpy(msg + ptr, checksum.data(), sizeof(uint8_t) * 32);

    return byteArray;
}

void BlockMsg::print() const {
    std::cout << "BlockMsg\n";
    m_block->print();
    std::cout << std::endl;
}

TxMsg::TxMsg() {
    m_comm = MsgTypes::Tx;
    m_tx = nullptr;
}

TxMsg::TxMsg(std::unique_ptr<Transaction>& tx) {
    m_comm = MsgTypes::Tx;
    m_tx = std::make_unique<Transaction>(tx.get());
}

Transaction* TxMsg::getTransaction() {
    return m_tx.get();
}

void TxMsg::parse(uint8_t* data, size_t size) {
    size_t headerSize = sizeof(char) * STARTSIZE + sizeof(uint8_t) + sizeof(size_t) +
        sizeof(uint32_t) * 8;
    size_t payload_size;
    memcpy(&payload_size, data + STARTSIZE + sizeof(uint8_t), sizeof(size_t));
    size_t ptr = headerSize;
    m_tx.release();
    m_tx.reset(new Transaction(0, 0, 0));
    m_tx->decode(data + ptr);
}

std::vector<uint8_t> TxMsg::toByte() const {
    size_t payloadSize = m_tx->size();
    size_t headerSize = sizeof(char) * STARTSIZE + sizeof(uint8_t) + sizeof(size_t) +
        sizeof(uint32_t) * 8;
    size_t size = headerSize + payloadSize;

    std::vector<uint8_t> byteArray(size);
    uint8_t* msg = byteArray.data();
    size_t ptr = 0;

    // create header

    memcpy(msg + ptr, STARTSTR, STARTSIZE);
    ptr += STARTSIZE;
    memcpy(msg + ptr, &m_comm, sizeof(uint8_t));
    ptr += sizeof(uint8_t);
    memcpy(msg + ptr, &size, sizeof(size_t));
    ptr += sizeof(size_t);

    // create Payload
    ptr = headerSize;
    m_tx->encode(msg + ptr);

    // create Checksum

    CryptoppImpl cryptor;;
    std::array<uint8_t, 32> checksum = cryptor.sha256Hash(
        std::string(reinterpret_cast<char*>(msg + headerSize), payloadSize));
    ptr = sizeof(char) * STARTSIZE + sizeof(uint8_t) + sizeof(size_t);
    memcpy(msg + ptr, checksum.data(), sizeof(uint32_t) * 8);

    return byteArray;
}

void TxMsg::print() const {
    std::cout << "TxMsg\n";
    m_tx->print();
    std::cout << std::endl;
}

NoFoundMsg::NoFoundMsg() {
    m_comm = MsgTypes::noFound;
}

void NoFoundMsg::parse(uint8_t* data, size_t size) {
}

std::vector<uint8_t> NoFoundMsg::toByte() const {
    size_t payloadSize = 0;
    size_t headerSize = sizeof(char) * STARTSIZE + sizeof(uint8_t) + sizeof(size_t) +
        sizeof(uint32_t) * 8;
    size_t size = headerSize + payloadSize;

    std::vector<uint8_t> byteArray(size);
    uint8_t* msg = byteArray.data();
    size_t ptr = 0;

    // create header

    memcpy(msg + ptr, STARTSTR, STARTSIZE);
    ptr += STARTSIZE;
    memcpy(msg + ptr, &m_comm, sizeof(uint8_t));
    ptr += sizeof(uint8_t);
    memcpy(msg + ptr, &size, sizeof(size_t));
    ptr += sizeof(size_t);

    // create Checksum
    CryptoppImpl cryptor;
    std::array<uint8_t, 32> checksum = cryptor.sha256Hash("12345678");

    ptr = sizeof(char) * STARTSIZE + sizeof(uint8_t) + sizeof(size_t);
    memcpy(msg + ptr, checksum.data(), sizeof(uint8_t) * 32);

    return byteArray;
}

void NoFoundMsg::print() const {
    std::cout << "NoFoundMsg\n";
    std::cout << std::endl;
}
