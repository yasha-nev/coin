#include "Message.hpp"

#define STARTSIZE 9

Message::~Message(){
    
}

uint8_t Message::getCommand(){
    return m_comm;
}

void Message::setClientId(int id){
    m_clientId = id;
}

int Message::getClientId(){
    return m_clientId;
}

GetBlocksMsg::GetBlocksMsg(){
    m_comm = MsgTypes::gBlocks;
    m_ver = 0;
}

GetBlocksMsg::GetBlocksMsg(std::list<std::array<uint32_t, 8>> hashes){
    m_comm = MsgTypes::gBlocks;
    m_ver = 0;
    m_hashes = hashes;
}

std::list<std::array<uint32_t, 8>> GetBlocksMsg::getHashes(){
    return m_hashes;
}

void GetBlocksMsg::parse(uint8_t *data, size_t size){
    size_t headerSize = sizeof(char) * STARTSIZE + sizeof(uint8_t) + sizeof(size_t) + sizeof(uint32_t) * 8;
    size_t count = 0;
    size_t payload_size;
    memcpy(&payload_size, data + sizeof(char) * STARTSIZE + sizeof(uint8_t), sizeof(size_t));
    size_t ptr = headerSize;
    memcpy(&m_ver, data + ptr, sizeof(uint8_t)); ptr += sizeof(uint8_t);
    memcpy(&count, data + ptr, sizeof(size_t));  ptr += sizeof(size_t);
    
    for (int i = 0; i < count - 1; i++){
        std::array<uint32_t, 8> hash;
        for (int j = 0; j < 8; j++){
            uint32_t h;
            memcpy(&h, data + ptr + j * sizeof(uint32_t), sizeof(uint32_t));
            hash[j] = h;
        }
        m_hashes.push_back(hash);
        ptr += sizeof(uint32_t) * 8;
    }
}

uint8_t *GetBlocksMsg::toByte(size_t &size){
    size_t count = m_hashes.size() + 1;
    size_t payloadSize = sizeof(uint8_t) + sizeof(size_t) + sizeof(uint32_t) * 8 * (m_hashes.size() + 1);
    size_t headerSize = sizeof(char) * STARTSIZE + sizeof(uint8_t) + sizeof(size_t) + sizeof(uint32_t) * 8;
    size = headerSize + payloadSize;
    
    uint8_t *msg = new uint8_t[size];
    size_t ptr = 0;
    
    //create header
    
    memcpy(msg + ptr, STARTSTR, STARTSIZE); ptr += STARTSIZE;
    memcpy(msg + ptr, &m_comm, sizeof(uint8_t)); ptr += sizeof(uint8_t);
    memcpy(msg + ptr, &payloadSize, sizeof(size_t)); ptr += sizeof(size_t);
    
    ptr = headerSize;
    
    //create payload
    
    memcpy(msg + ptr, &m_ver, sizeof(uint8_t)); ptr += sizeof(uint8_t);
    memcpy(msg + ptr, &count, sizeof(size_t)); ptr += sizeof(size_t);
    
    for (std::array<uint32_t, 8> hash : m_hashes){
        for (int j = 0; j < 8; j++){
            uint32_t h = hash[j];
            memcpy(msg + ptr + sizeof(uint32_t) * j, &h, sizeof(uint32_t));
        }
        
        ptr += sizeof(uint32_t) * 8;
    }
    
    uint32_t zerohash[8] = {0, 0, 0, 0, 0, 0, 0, 0};
    memcpy(msg + ptr, zerohash, sizeof(uint32_t) * 8);
    ptr += sizeof(uint32_t) * 8;
    
    
    //create checksum
    
    sha256 sha;
    std::array<uint32_t, 8> checksum = sha.Hash(std::string(reinterpret_cast<char *>(msg + headerSize), payloadSize));
    ptr = sizeof(char) * STARTSIZE + sizeof(uint8_t) + sizeof(size_t);
    memcpy(msg + ptr, checksum.data(), sizeof(uint32_t) * 8);
    
    return msg;
}

InvMsg::InvMsg(){
    m_comm = MsgTypes::Inv;
    m_type = 0;
}

InvMsg::InvMsg(InvTypes type, std::list<std::array<uint32_t, 8>> hashes){
    m_comm = MsgTypes::Inv;
    m_hashes = hashes;
    m_type = static_cast<uint8_t>(type);
}

std::list<std::array<uint32_t, 8>> InvMsg::getHashes(){
    return m_hashes;
}

void InvMsg::parse(uint8_t *data, size_t size){
    size_t headerSize = sizeof(char) * STARTSIZE + sizeof(uint8_t) + sizeof(size_t) + sizeof(uint32_t) * 8;
    size_t count = 0;
    size_t payload_size;
    memcpy(&payload_size, data + sizeof(char) * STARTSIZE + sizeof(uint8_t), sizeof(size_t));
    
    size_t ptr = headerSize;
    memcpy(&count, data + ptr, sizeof(size_t));  ptr += sizeof(size_t);
    
    for (int i = 0; i < count - 1; i++){
        memcpy(&m_type, data + ptr, sizeof(uint8_t)); ptr += sizeof(uint8_t);
        std::array<uint32_t, 8> hash;
        for (int j = 0; j < 8; j++){
            uint32_t h;
            memcpy(&h, data + ptr + j * sizeof(uint32_t), sizeof(uint32_t));
            hash[j] = h;
        }
        
        m_hashes.push_back(hash);
        ptr += sizeof(uint32_t) * 8;
    }
}

uint8_t *InvMsg::toByte(size_t &size){
    size_t count = m_hashes.size() + 1;
    size_t payloadSize = sizeof(size_t) + sizeof(uint8_t) * (m_hashes.size()) + sizeof(uint32_t) * 8 * (m_hashes.size());
    size_t headerSize = sizeof(char) * STARTSIZE + sizeof(uint8_t) + sizeof(size_t) + sizeof(uint32_t) * 8;
    size = headerSize + payloadSize;

    uint8_t *msg = new uint8_t[size];
    size_t ptr = 0;
    
    //create header
    
    memcpy(msg + ptr, STARTSTR, STARTSIZE); ptr += STARTSIZE;
    memcpy(msg + ptr, &m_comm, sizeof(uint8_t)); ptr += sizeof(uint8_t);
    memcpy(msg + ptr, &size, sizeof(size_t)); ptr += sizeof(size_t);
    
    //create Payload
    ptr = headerSize;

    memcpy(msg + ptr, &count, sizeof(size_t)); ptr += sizeof(size_t);
    for (std::array<uint32_t, 8> hash : m_hashes){
        memcpy(msg + ptr, &m_type, sizeof(uint8_t)); ptr += sizeof(uint8_t);
        
        for (int j = 0; j < 8; j++){
            uint32_t h = hash[j];
            memcpy(msg + ptr + sizeof(uint32_t) * j, &h, sizeof(uint32_t));
        }
        
        ptr += sizeof(uint32_t) * 8;
    }
    
    // create Checksum
    
    sha256 sha;
    std::array<uint32_t, 8> checksum = sha.Hash(std::string(reinterpret_cast<char *>(msg + headerSize), payloadSize));
    ptr = sizeof(char) * STARTSIZE + sizeof(uint8_t) + sizeof(size_t);
    memcpy(msg + ptr, checksum.data(), sizeof(uint32_t) * 8);
    
    return msg;
}

GetDataMsg::GetDataMsg(){
    m_comm = MsgTypes::gData;
    m_type = 0;
}

GetDataMsg::GetDataMsg(DataTypes type, std::list<std::array<uint32_t, 8>> hashes){
    m_comm = MsgTypes::gData;
    m_hashes = hashes;
    m_type = static_cast<uint8_t>(type);
}

std::list<std::array<uint32_t, 8>> GetDataMsg::getHashes(){
    return m_hashes;
}

void GetDataMsg::parse(uint8_t *data, size_t size){
    size_t headerSize = sizeof(char) * STARTSIZE + sizeof(uint8_t) + sizeof(size_t) + sizeof(uint32_t) * 8;
    size_t count = 0;
    size_t payload_size;
    memcpy(&payload_size, data + sizeof(char) * STARTSIZE + sizeof(uint8_t), sizeof(size_t));
    
    size_t ptr = headerSize;
    memcpy(&count, data + ptr, sizeof(size_t));  ptr += sizeof(size_t);
    
    for (int i = 0; i < count - 1; i++){
        memcpy(&m_type, data + ptr, sizeof(uint8_t)); ptr += sizeof(uint8_t);
        std::array<uint32_t, 8> hash;
        for (int j = 0; j < 8; j++){
            uint32_t h;
            memcpy(&h, data + ptr + j * sizeof(uint32_t), sizeof(uint32_t));
            hash[j] = h;
        }
        
        m_hashes.push_back(hash);
        ptr += sizeof(uint32_t) * 8;
    }
}

uint8_t *GetDataMsg::toByte(size_t &size){
    size_t count = m_hashes.size() + 1;
    size_t payloadSize = sizeof(size_t) + sizeof(uint8_t) * (m_hashes.size()) + sizeof(uint32_t) * 8 * (m_hashes.size());
    size_t headerSize = sizeof(char) * STARTSIZE + sizeof(uint8_t) + sizeof(size_t) + sizeof(uint32_t) * 8;
    size = headerSize + payloadSize;

    uint8_t *msg = new uint8_t[size];
    size_t ptr = 0;
    
    //create header
    
    memcpy(msg + ptr, STARTSTR, STARTSIZE); ptr += STARTSIZE;
    memcpy(msg + ptr, &m_comm, sizeof(uint8_t)); ptr += sizeof(uint8_t);
    memcpy(msg + ptr, &size, sizeof(size_t)); ptr += sizeof(size_t);
    
    //create Payload
    ptr = headerSize;

    memcpy(msg + ptr, &count, sizeof(size_t)); ptr += sizeof(size_t);
    for (std::array<uint32_t, 8> hash : m_hashes){
        memcpy(msg + ptr, &m_type, sizeof(uint8_t)); ptr += sizeof(uint8_t);
        
        for (int j = 0; j < 8; j++){
            uint32_t h = hash[j];
            memcpy(msg + ptr + sizeof(uint32_t) * j, &h, sizeof(uint32_t));
        }
        
        ptr += sizeof(uint32_t) * 8;
    }
    
    // create Checksum
    
    sha256 sha;
    std::array<uint32_t, 8> checksum = sha.Hash(std::string(reinterpret_cast<char *>(msg + headerSize), payloadSize));
    ptr = sizeof(char) * STARTSIZE + sizeof(uint8_t) + sizeof(size_t);
    memcpy(msg + ptr, checksum.data(), sizeof(uint32_t) * 8);
    
    return msg;
}

BlockMsg::BlockMsg(){
    m_comm = MsgTypes::sBlock;
    m_block = nullptr;
}

BlockMsg::BlockMsg(Block *block){
    m_comm = MsgTypes::sBlock;
    m_block = block;
}

Block  *BlockMsg::getBlock(){
    return m_block;
}

void BlockMsg::parse(uint8_t *data, size_t size){
    size_t headerSize = sizeof(char) * STARTSIZE + sizeof(uint8_t) + sizeof(size_t) + sizeof(uint32_t) * 8;
    size_t payload_size;
    memcpy(&payload_size, data + sizeof(char) * STARTSIZE + sizeof(uint8_t), sizeof(size_t));
    
    size_t ptr = headerSize;
    
    if (m_block){
        delete m_block;
    }
    
    m_block = decode(data + ptr);
}

uint8_t *BlockMsg::toByte(size_t &size){
    size_t payloadSize = m_block->size();
    size_t headerSize = sizeof(char) * STARTSIZE + sizeof(uint8_t) + sizeof(size_t) + sizeof(uint32_t) * 8;
    size = headerSize + payloadSize;

    uint8_t *msg = new uint8_t[size];
    size_t ptr = 0;
    
    //create header
    
    memcpy(msg + ptr, STARTSTR, STARTSIZE); ptr += STARTSIZE;
    memcpy(msg + ptr, &m_comm, sizeof(uint8_t)); ptr += sizeof(uint8_t);
    memcpy(msg + ptr, &size, sizeof(size_t)); ptr += sizeof(size_t);
    
    //create Payload
    ptr = headerSize;
    
    size_t blocksize = 0;
    
    uint8_t *encBlock = m_block->encode(&blocksize);

    memcpy(msg + ptr, encBlock, blocksize); ptr += blocksize;
    
    delete[] encBlock;
    
    // create Checksum
    
    sha256 sha;
    std::array<uint32_t, 8> checksum = sha.Hash(std::string(reinterpret_cast<char *>(msg + headerSize), payloadSize));
    ptr = sizeof(char) * STARTSIZE + sizeof(uint8_t) + sizeof(size_t);
    memcpy(msg + ptr, checksum.data(), sizeof(uint32_t) * 8);
    
    return msg;
}

TxMsg::TxMsg(){
    m_comm = MsgTypes::Tx;
    m_tx = nullptr;
}

TxMsg::TxMsg(Transaction *tx){
    m_comm = MsgTypes::Tx;
    m_tx = tx;
}

Transaction* TxMsg::getTransaction(){
    return m_tx;
}

void TxMsg::parse(uint8_t *data, size_t size){
    size_t headerSize = sizeof(char) * STARTSIZE + sizeof(uint8_t) + sizeof(size_t) + sizeof(uint32_t) * 8;
    size_t payload_size;
    memcpy(&payload_size, data + sizeof(char) * STARTSIZE + sizeof(uint8_t), sizeof(size_t));
    
    size_t ptr = headerSize;
    
    if (m_tx){
        delete m_tx;
    }
    
    m_tx = new Transaction(0, 0, 0);
    
    m_tx->decode(data + ptr);
}

uint8_t *TxMsg::toByte(size_t &size){
    size_t payloadSize = m_tx->size();
    size_t headerSize = sizeof(char) * STARTSIZE + sizeof(uint8_t) + sizeof(size_t) + sizeof(uint32_t) * 8;
    size = headerSize + payloadSize;

    uint8_t *msg = new uint8_t[size];
    size_t ptr = 0;
    
    //create header
    
    memcpy(msg + ptr, STARTSTR, STARTSIZE); ptr += STARTSIZE;
    memcpy(msg + ptr, &m_comm, sizeof(uint8_t)); ptr += sizeof(uint8_t);
    memcpy(msg + ptr, &size, sizeof(size_t)); ptr += sizeof(size_t);
    
    //create Payload
    ptr = headerSize;
    
    m_tx->encode(msg + ptr);
    
    // create Checksum
    
    sha256 sha;
    std::array<uint32_t, 8> checksum = sha.Hash(std::string(reinterpret_cast<char *>(msg + headerSize), payloadSize));
    ptr = sizeof(char) * STARTSIZE + sizeof(uint8_t) + sizeof(size_t);
    memcpy(msg + ptr, checksum.data(), sizeof(uint32_t) * 8);
    
    return msg;
}

NoFoundMsg::NoFoundMsg(){
    m_comm = MsgTypes::noFound;
}

void NoFoundMsg::parse(uint8_t *data, size_t size){
}

uint8_t *NoFoundMsg::toByte(size_t &size){
    size_t payloadSize = 0;
    size_t headerSize = sizeof(char) * STARTSIZE + sizeof(uint8_t) + sizeof(size_t) + sizeof(uint32_t) * 8;
    size = headerSize + payloadSize;
    
    uint8_t *msg = new uint8_t[size];
    size_t ptr = 0;
    
    //create header
    
    memcpy(msg + ptr, STARTSTR, STARTSIZE); ptr += STARTSIZE;
    memcpy(msg + ptr, &m_comm, sizeof(uint8_t)); ptr += sizeof(uint8_t);
    memcpy(msg + ptr, &size, sizeof(size_t)); ptr += sizeof(size_t);
    
    // create Checksum
    sha256 sha;
    std::array<uint32_t, 8> checksum = sha.Hash("12345678");
    
    ptr = sizeof(char) * STARTSIZE + sizeof(uint8_t) + sizeof(size_t);
    memcpy(msg + ptr, checksum.data(), sizeof(uint32_t) * 8);
    
    return msg;
}
