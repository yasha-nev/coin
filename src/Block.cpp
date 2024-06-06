#include "Block.hpp"

Block::Block(const int64_t &timeStamp, 
             const std::list<Transaction> &tx,
             const std::array<uint32_t, 8> &prevBlockHash,
             const std::array<uint32_t, 8> &hash,
             const int64_t &nonce){
    m_timeStamp = timeStamp;
    m_tx = tx;
    m_prevBlockHash = prevBlockHash;
    m_hash = hash;
    m_nonce = nonce;
}

Block::Block(){
    m_timeStamp = 0;
    m_nonce = 0;
}

Block::~Block(){
}

uint64_t Block::getTimeStamp(){
    return m_timeStamp;
}

std::list<Transaction> Block::getTransaction(){
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

void Block::setTransaction(std::list<Transaction> &tx){
    m_tx = tx;
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
    
    size_t txsize = 0;
    for (auto &tx: m_tx){
        txsize += tx.size();
    }
    
    return sizeof(uint64_t) +
    sizeof(size_t) +
    txsize +
    sizeof(uint32_t) * 8 +
    sizeof(uint32_t) * 8 +
    sizeof(uint64_t);
}

uint8_t *Block::encode(size_t *blockSize){
    *blockSize = size();
    
    uint8_t *enc = new uint8_t[*blockSize];
    uint8_t *ptr = enc;
    
    memcpy(ptr, &m_timeStamp, sizeof(uint64_t)); ptr += sizeof(uint64_t);
    
    size_t txnum = m_tx.size();
    
    memcpy(ptr, &txnum, sizeof(size_t)); ptr += sizeof(size_t);
    
    for (auto &tx : m_tx){
        tx.encode(ptr); 
        ptr += tx.size();
    }
    
    memcpy(ptr, m_prevBlockHash.data(), sizeof(uint32_t) * 8); ptr += sizeof(uint32_t) * 8;
    
    memcpy(ptr, m_hash.data(), sizeof(uint32_t) * 8); ptr += sizeof(uint32_t) * 8;
    
    memcpy(ptr, &m_nonce, sizeof(uint64_t));
    
    return enc;
}

Block* decode(uint8_t *dec){
    Block *block = new Block();
    uint8_t *ptr = dec;
    
    memcpy(&block->m_timeStamp, ptr, sizeof(uint64_t)); ptr += sizeof(uint64_t);
    
    size_t txnum = 0;
    
    memcpy(&txnum, ptr, sizeof(size_t)); ptr += sizeof(size_t);
    
    for (size_t i = 0; i < txnum; i++){
        Transaction tx = Transaction(0, 0, 0);
        tx.decode(ptr);
        
        block->m_tx.push_back(tx); ptr += tx.size();
    }
    
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
    
    for (auto &tx : m_tx){
        tx.print();
    }
    std::cout <<std::setfill('=') << std::setw(80) << "\n" << std::endl;
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
    
    for (auto &tx : m_block->m_tx){
        data += tx.toString();
    }
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
