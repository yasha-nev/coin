#include "Block.hpp"

Block::Block(const int64_t &timeStamp, Transaction *tx, const std::array<uint32_t, 8> &prevBlockHash, const std::array<uint32_t, 8> &hash, const int64_t &nonce){
    m_timeStamp = timeStamp;
    m_tx = new Transaction(tx);
    m_prevBlockHash = prevBlockHash;
    m_hash = hash;
    m_nonce = nonce;
}

Block::Block(){
    m_timeStamp = 0;
    m_tx = new Transaction(0, 0, 0);
    m_nonce = 0;
}

Block::~Block(){
    if (m_tx){
        delete m_tx;
    }
}

uint64_t Block::getTimeStamp(){
    return m_timeStamp;
}

Transaction *Block::getTransaction(){
    return m_tx;
}

std::array<uint32_t, 8> Block::getPrevBlockHash(){
    return m_prevBlockHash;
}

std::array<uint32_t, 8> Block::getHash(){
    return m_hash;
}

uint64_t Block::getNonce(){
    return m_nonce;
}

void Block::setTimeStamp(const int64_t &timeStamp){
    m_timeStamp = timeStamp;
}

void Block::setTransaction(Transaction *tx){
    delete m_tx;
    m_tx = new Transaction(tx);
}

void Block::setNonce(const uint64_t &nonce){
    m_nonce = nonce;
}

void Block::setPrevBlockHash(const std::array<uint32_t, 8> &hash){
    m_prevBlockHash = hash;
}

void Block::setHash(const std::array<uint32_t, 8> &hash){
    m_hash = hash;
}

size_t Block::size(){
    return sizeof(uint64_t) +
    m_tx->size() +
    sizeof(uint32_t) * 8 +
    sizeof(uint32_t) * 8 +
    sizeof(uint64_t);
}

uint8_t *Block::encode(size_t *blockSize){
    *blockSize = sizeof(uint64_t) +
        m_tx->size() +  
        sizeof(uint32_t) * 8 +
        sizeof(uint32_t) * 8 +
        sizeof(uint64_t);
    
    uint8_t *enc = new uint8_t[*blockSize];
    uint8_t *ptr = enc;
    
    memcpy(ptr, &m_timeStamp, sizeof(uint64_t));
    ptr += sizeof(uint64_t);
    
    m_tx->encode(ptr);
    
    ptr += m_tx->size();
    
    memcpy(ptr, m_prevBlockHash.data(), sizeof(uint32_t) * 8);
    ptr += sizeof(uint32_t) * 8;
    
    memcpy(ptr, m_hash.data(), sizeof(uint32_t) * 8);
    ptr += sizeof(uint32_t) * 8;
    
    memcpy(ptr, &m_nonce, sizeof(uint64_t));
    
    return enc;
}

Block* decode(uint8_t *dec){
    Block *block = new Block();
    uint8_t *ptr = dec;
    
    memcpy(&block->m_timeStamp, ptr, sizeof(uint64_t));
    ptr += sizeof(uint64_t);
    
    block->m_tx->decode(ptr);
    
    ptr += block->m_tx->size();
    
    uint32_t *ptr32 = (uint32_t *)ptr;
    
    for (int i = 0; i < 8; i++){
        block->m_prevBlockHash[i] = *ptr32;
        ptr32++;
    }
    
    ptr += sizeof(uint32_t) * 8;
    
    ptr32 = (uint32_t *)ptr;
    
    for (int i = 0; i < 8; i++){
        block->m_hash[i] = *ptr32;
        ptr32++;
    }
    
    ptr += sizeof(uint32_t) * 8;

    memcpy(&block->m_nonce, ptr, sizeof(uint64_t));
    
    return block;
}

void Block::print(){
    std::cout << std::setfill('=') << std::setw(40) << "BLOCK" << std::setfill('=') << std::setw(40) << "\n";
    std::cout << "|time: "  << m_timeStamp << "\n";
    std::cout << "|Nonce: " <<  m_nonce << "\n";
    std::cout << "|Hash: " <<  array2String(m_hash) << "\n";
    std::cout << "|PrevHash: " << array2String(m_prevBlockHash) << "\n";
    std::cout << "|transaction id: "  << m_tx->m_id << "\n";
    std::cout << "|" << std::setfill('_') << std::setw(39) << "TXINPUTS" << std::setfill('_') << std::setw(40) << "\n";
    
    for (int i = 0; i < m_tx->m_inCount; i++){
        std::cout << "|output id : "  << m_tx->m_in[i].m_tranId << "\n";
        std::cout << "|index: "  <<  m_tx->m_in[i].m_outIndex << "\n";
        std::cout << "|pubkey from: "  <<  m_tx->m_in[i].m_pubkey << "\n";
        std::cout << "|" << std::setfill('-') << std::setw(79) << "\n";
    }
    std::cout << "|" << std::setfill('_') << std::setw(39) << "TXOUTPUTS" << std::setfill('_') << std::setw(40) << "\n";
    for (int i = 0; i < m_tx->m_outCount; i++){
        std::cout << "|value: " <<  m_tx->m_out[i].m_value << "\n";
        std::cout << "|pubkey to: " <<  m_tx->m_out[i].m_pubkey << "\n";
        std::cout << "|" << std::setfill('-') << std::setw(79) << "\n";
    }
    
    std::cout <<std::setfill('=') << std::setw(80) << "\n" << std::endl;;
}

void printBigInt(uint32_t *bigint){
    for (int i = 0 ; i < 8; i++){
        std::cout <<  std::setfill('0') << std::setw(8) << std::hex << bigint[i];
    }
    std::cout << "\n";
    
}

std::string array2String(const std::array<uint32_t, 8> &arr) {
    std::stringstream s;

    for(int i = 0 ; i < 8 ; i++) {
        s << std::setfill('0') << std::setw(8) << std::hex << arr[i];
    }
    return s.str();
}

static int bigIntCmp(const std::array<uint32_t, 8> &left, const std::array<uint32_t, 8> &right){
    for (int i = 0; i < 8; i++){
        if (left[i] > right[i]){
            return 1;
        }
        else if (left[i] < right[i]){
            return -1;
        }
    }
    return 0;
}

ProofOfWork::ProofOfWork(class Block *block){
    m_nonce = 0;
    m_block = block;
    m_target[0] = 1 << 16;
}

std::string ProofOfWork::PrepareData(){
    std::string data;
    data += std::to_string(m_block->getTimeStamp());
    data += m_block->m_tx->toString();
    data += array2String(m_block->getPrevBlockHash());
    data += std::to_string(m_nonce);
    return data;
}

void ProofOfWork::Run(){
    m_nonce = 0;
    std::array<uint32_t, 8> hash;
    sha256 Crypto;
    std::cout << "==============Block Hashing==============\n";
    while (m_nonce < MAXNONCE){
        std::string data = PrepareData();
        
        hash = Crypto.Hash(data);
        
        if (m_nonce % 100000 == 0){
            std::cout << ">";
        }
        
        if (bigIntCmp(hash, m_target) == -1){
            m_block->setNonce(m_nonce);
            m_block->m_hash = hash;
        }
        m_nonce += 1;
    }
    
    
    std::cout << "\n" << std::endl;
}
