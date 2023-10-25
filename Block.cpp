#include "Block.hpp"

Block::Block(int64_t timeStamp, string data, uint32_t *prevBlockHash, uint32_t *hash, int64_t nonce){
    m_timeStamp = timeStamp;
    m_data = data;
    m_prevBlockHash = new uint32_t[8];
    m_hash = new uint32_t[8];
    m_nonce = nonce;
    memcpy(m_prevBlockHash, prevBlockHash, sizeof(uint32_t) * 8);
    memcpy(m_hash, hash, sizeof(uint32_t) * 8);
}

Block::Block(){
    m_timeStamp = 0;
    m_data = "";
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
}

uint64_t Block::getTimeStamp(){
    return m_timeStamp;
}

string Block::getData(){
    return m_data;
}

uint32_t *Block::getPrevBlockHash(){
    return m_prevBlockHash;
}

uint32_t *Block::getHash(){
    return m_hash;
}

int64_t Block::getNonce(){
    return m_nonce;
}

void Block::setTimeStamp(int64_t timeStamp){
    m_timeStamp = timeStamp;
}

void Block::setData(string data){
    m_data = data;
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
        sizeof(int) + sizeof(char) * m_data.size() +
        sizeof(uint32_t) * 8 +
        sizeof(uint32_t) * 8 +
        sizeof(int);
    
    uint8_t *enc = new uint8_t[*blockSize];
    uint8_t *ptr = enc;
    uint64_t char_count = m_data.size();
    
    std::memcpy(ptr, &m_timeStamp, sizeof(uint64_t));
    ptr += sizeof(uint64_t);
    
    std::memcpy(ptr, &char_count, sizeof(uint64_t));
    ptr += sizeof(uint64_t);
    
    for (int i = 0; i < m_data.size(); i++){
         *ptr = m_data[i];
        ptr += sizeof(uint8_t);
    }
    
    std::memcpy(ptr, m_prevBlockHash, sizeof(uint32_t) * 8);
    ptr += sizeof(uint32_t) * 8;
    
    std::memcpy(ptr, m_hash, sizeof(uint32_t) * 8);
    ptr += sizeof(uint32_t) * 8;
    
    std::memcpy(ptr, &m_nonce, sizeof(int));
        
    return enc;
}

Block* decode(uint8_t *dec){
    Block *block = new Block();
    uint8_t *ptr = dec;
    uint64_t char_count = 0;
    
    std::memcpy(&block->m_timeStamp, ptr, sizeof(uint64_t));
    ptr += sizeof(uint64_t);
    
    std::memcpy(&char_count, ptr, sizeof(uint64_t));
    ptr += sizeof(uint64_t);
    
    // Это вообще веселая штука, я не знаю как сделать чтоб memcpy корректно копироват char объекты без артефактов
    // std::memcpy(str, ptr, sizeof(str) / sizeof(str[0]));
    // Потому что char[] = "abc" => {'a', 'b', 'c', '\0'}
    // пока так
    
    block->m_data = "";
    for (int i = 0; i < char_count; i++){
        block->m_data += (char) *ptr;
        ptr += sizeof(uint8_t);
    }
    
    std::memcpy(block->m_prevBlockHash, ptr, sizeof(uint32_t) * 8);
    ptr += sizeof(uint32_t) * 8;
    
    std::memcpy(block->m_hash, ptr, sizeof(uint32_t) * 8);
    ptr += sizeof(uint32_t) * 8;

    std::memcpy(&block->m_nonce, ptr, sizeof(int));
    
    return block;
}

void Block::print(){
    cout << "time: " << m_timeStamp << "\n";
    cout << "Data: " << m_data << "\n";
    cout << "Nonce: " << m_nonce << "\n";
    cout << "Hash: " << array2String(m_hash) << "\n";
    cout << "PrevHash: " <<array2String(m_prevBlockHash) << "\n\n";
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
    data += m_block->getData();
    data += array2String(m_block->getPrevBlockHash());
    data += to_string(m_nonce);
    return data;
}

void ProofOfWork::Run(){
    m_nonce = 0;
    uint32_t *hash;
    sha256 Crypto;
    
    while (m_nonce < MAXNONCE){
        string data = PrepareData();
        hash = Crypto.Hash(data);
        
        if (m_nonce % 100000 == 0){
            cout << m_nonce << endl;
        }
        
        if (bigIntCmp(hash, m_target) == -1){
            m_block->setNonce(m_nonce);
            
            memcpy(m_block->m_hash, hash, sizeof(uint32_t) * 8);
        }
        m_nonce += 1;
    }
}
