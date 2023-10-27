#include "Block.hpp"

Block::Block(int64_t timeStamp, Transaction *tx, uint32_t *prevBlockHash, uint32_t *hash, int64_t nonce){
    m_timeStamp = timeStamp;
    m_tx = new Transaction(tx);
    m_prevBlockHash = new uint32_t[8];
    m_hash = new uint32_t[8];
    m_nonce = nonce;
    memcpy(m_prevBlockHash, prevBlockHash, sizeof(uint32_t) * 8);
    memcpy(m_hash, hash, sizeof(uint32_t) * 8);
}

Block::Block(){
    m_timeStamp = 0;
    m_tx = new Transaction(0, 0, 0);
    m_prevBlockHash = new uint32_t[8];
    m_hash = new uint32_t[8];
    m_nonce = 0;
}

Block::~Block(){
    if (m_hash){
        delete[] m_hash;
    }
    
    if (m_prevBlockHash){
        delete[] m_prevBlockHash;
    }
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

uint32_t *Block::getPrevBlockHash(){
    return m_prevBlockHash;
}

uint32_t *Block::getHash(){
    return m_hash;
}

uint64_t Block::getNonce(){
    return m_nonce;
}

void Block::setTimeStamp(int64_t timeStamp){
    m_timeStamp = timeStamp;
}

void Block::setTransaction(Transaction *tx){
    delete m_tx;
    m_tx = new Transaction(tx);
}

void Block::setNonce(uint64_t nonce){
    m_nonce = nonce;
}

void Block::setPrevBlockHash(uint32_t *hash){
    memcpy(m_prevBlockHash, hash, sizeof(uint32_t) * 8);
}

void Block::setHash(uint32_t *hash){
    memcpy(m_hash, hash, sizeof(uint32_t) * 8);
}

uint8_t *Block::encode(size_t *blockSize){
    *blockSize = sizeof(uint64_t) +
        m_tx->size() +  
        sizeof(uint32_t) * 8 +
        sizeof(uint32_t) * 8 +
        sizeof(uint64_t);
    
    uint8_t *enc = new uint8_t[*blockSize];
    uint8_t *ptr = enc;
    
    std::memcpy(ptr, &m_timeStamp, sizeof(uint64_t));
    ptr += sizeof(uint64_t);
    
    m_tx->encode(ptr);
    
    ptr += m_tx->size();
    
    std::memcpy(ptr, m_prevBlockHash, sizeof(uint32_t) * 8);
    ptr += sizeof(uint32_t) * 8;
    
    std::memcpy(ptr, m_hash, sizeof(uint32_t) * 8);
    ptr += sizeof(uint32_t) * 8;
    
    std::memcpy(ptr, &m_nonce, sizeof(uint64_t));
    
    return enc;
}

Block* decode(uint8_t *dec){
    Block *block = new Block();
    uint8_t *ptr = dec;
    
    std::memcpy(&block->m_timeStamp, ptr, sizeof(uint64_t));
    ptr += sizeof(uint64_t);
    
    block->m_tx->decode(ptr);
    
    ptr += block->m_tx->size();
    
    std::memcpy(block->m_prevBlockHash, ptr, sizeof(uint32_t) * 8);
    ptr += sizeof(uint32_t) * 8;
    
    std::memcpy(block->m_hash, ptr, sizeof(uint32_t) * 8);
    ptr += sizeof(uint32_t) * 8;

    std::memcpy(&block->m_nonce, ptr, sizeof(uint64_t));
    
    return block;
}

void Block::print(){
    cout <<setfill('=') << setw(40) << "BLOCK" << setfill('=') << setw(40) << "\n";
    cout << "|time: "  << m_timeStamp << "\n";
    cout << "|transaction id: "  << m_tx->id << "\n";
    cout << "|Nonce: " <<  m_nonce << "\n";
    cout << "|Hash: " <<  array2String(m_hash) << "\n";
    cout << "|PrevHash: " << array2String(m_prevBlockHash) << "\n";
    cout << "|" << setfill('_') << setw(39) << "TXINPUTS" << setfill('_') << setw(40) << "\n";
    
    for (int i = 0; i < m_tx->in_count; i++){
        cout << "|output id : "  << m_tx->in[i].tranId << "\n";
        cout << "|index: "  <<  m_tx->in[i].outIndex << "\n";
        cout << "|pubkey from: "  <<  m_tx->in[i].pubkey << "\n";
        cout << "|" << setfill('-') << setw(79) << "\n";
    }
    cout << "|" << setfill('_') << setw(39) << "TXOUTPUTS" << setfill('_') << setw(40) << "\n";
    for (int i = 0; i < m_tx->out_count; i++){
        cout << "|value: " <<  m_tx->out[i].value << "\n";
        cout << "|pubkey to: " <<  m_tx->out[i].pubkey << "\n";
        cout << "|" << setfill('-') << setw(79) << "\n";
    }
    
    cout <<setfill('=') << setw(80) << "\n" << endl;;
}

void printBigInt(uint32_t *bigint){
    for (int i = 0 ; i < 8; i++){
        cout <<  std::setfill('0') << setw(8) << hex << bigint[i];
    }
    cout << "\n";
    
}

std::string array2String(uint32_t *arr) {
    std::stringstream s;

    for(int i = 0 ; i < 8 ; i++) {
        s << std::setfill('0') << std::setw(8) << std::hex << arr[i];
    }

    return s.str();
}

static int bigIntCmp(uint32_t *left, uint32_t *right){
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

ProofOfWork::ProofOfWork(struct Block *block){
    m_nonce = 0;
    m_block = block;
    m_target = new uint32_t[8];
    memset(m_target, 0, sizeof(uint32_t) * 8);
    m_target[0] = 1 << 16;
}

ProofOfWork::~ProofOfWork(){
    if (m_target){
        delete[] m_target;
    }
}

string ProofOfWork::PrepareData(){
    string data;
    data += to_string(m_block->getTimeStamp());
    data += m_block->m_tx->toString();
    data += array2String(m_block->getPrevBlockHash());
    data += to_string(m_nonce);
    return data;
}

void ProofOfWork::Run(){
    m_nonce = 0;
    uint32_t *hash;
    sha256 Crypto;
    cout << "==============Block Hashing==============\n";
    while (m_nonce < MAXNONCE){
        string data = PrepareData();
        hash = Crypto.Hash(data);
        
        if (m_nonce % 100000 == 0){
            cout << ">";
        }
        
        if (bigIntCmp(hash, m_target) == -1){
            m_block->setNonce(m_nonce);
            
            memcpy(m_block->m_hash, hash, sizeof(uint32_t) * 8);
        }
        m_nonce += 1;
    }
    
    
    cout << "\n" << endl;
}
