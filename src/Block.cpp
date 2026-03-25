#include "Block.hpp"

Block::Block(
    const int64_t& timeStamp,
    const std::list<Transaction>& tx,
    const std::array<uint8_t, 32>& prevBlockHash,
    const std::array<uint8_t, 32>& hash,
    const int64_t& nonce) {
    m_timeStamp = timeStamp;
    m_tx = tx;
    m_prevBlockHash = prevBlockHash;
    m_hash = hash;
    m_nonce = nonce;
}

Block::Block() {
    m_timeStamp = 0;
    m_nonce = 0;
}

Block::Block(Block* block) {
    m_timeStamp = block->m_timeStamp;
    m_tx = block->m_tx;
    m_prevBlockHash = block->m_prevBlockHash;
    m_hash = block->m_hash;
    m_nonce = block->m_nonce;
}

Block::~Block() {
}

uint64_t Block::getTimeStamp() const noexcept {
    return m_timeStamp;
}

const std::list<Transaction>& Block::getTransaction() const noexcept {
    return m_tx;
}

const std::array<uint8_t, 32>& Block::getPrevBlockHash() const noexcept {
    return m_prevBlockHash;
}

const std::array<uint8_t, 32>& Block::getHash() const noexcept {
    return m_hash;
}

uint64_t Block::getNonce() const noexcept {
    return m_nonce;
}

void Block::setTimeStamp(const int64_t& timeStamp) noexcept {
    m_timeStamp = timeStamp;
}

void Block::setTransaction(const std::list<Transaction>& tx) noexcept {
    m_tx = tx;
}

void Block::setNonce(const uint64_t& nonce) noexcept {
    m_nonce = nonce;
}

void Block::setPrevBlockHash(const std::array<uint8_t, 32>& hash) noexcept {
    m_prevBlockHash = hash;
}

void Block::setHash(const std::array<uint8_t, 32>& hash) noexcept {
    m_hash = hash;
}

size_t Block::size() const noexcept {

    size_t txsize = 0;
    for(const auto& tx: m_tx) {
        txsize += tx.size();
    }

    return sizeof(uint64_t) + sizeof(size_t) + txsize + sizeof(uint32_t) * 8 +
        sizeof(uint32_t) * 8 + sizeof(uint64_t);
}

void Block::encode(uint8_t* ptr) {
    memcpy(ptr, &m_timeStamp, sizeof(uint64_t));
    ptr += sizeof(uint64_t);

    size_t txnum = m_tx.size();

    memcpy(ptr, &txnum, sizeof(size_t));
    ptr += sizeof(size_t);

    for(auto& tx: m_tx) {
        tx.encode(ptr);
        ptr += tx.size();
    }

    memcpy(ptr, m_prevBlockHash.data(), sizeof(uint8_t) * 32);
    ptr += sizeof(uint8_t) * 32;

    memcpy(ptr, m_hash.data(), sizeof(uint8_t) * 32);
    ptr += sizeof(uint8_t) * 32;

    memcpy(ptr, &m_nonce, sizeof(uint64_t));
}

void Block::decode(uint8_t* ptr) {
    m_timeStamp = 0;

    memcpy(&m_timeStamp, ptr, sizeof(uint64_t));
    ptr += sizeof(uint64_t);

    size_t txnum = 0;
    memcpy(&txnum, ptr, sizeof(size_t));
    ptr += sizeof(size_t);

    m_tx.clear();
    for(size_t i = 0; i < txnum; i++) {
        Transaction tx = Transaction(0, 0, 0);
        tx.decode(ptr);

        m_tx.push_back(tx);
        ptr += tx.size();
    }

    for(int i = 0; i < 32; i++) {
        m_prevBlockHash[i] = *ptr;
        ptr++;
    }

    for(int i = 0; i < 32; i++) {
        m_hash[i] = *ptr;
        ptr++;
    }

    memcpy(&m_nonce, ptr, sizeof(uint64_t));
}

void Block::print() const noexcept {
    CryptoppImpl cryptor;
    std::cout << std::setfill('=') << std::setw(40) << "BLOCK" << std::setfill('=') << std::setw(40)
              << "\n";
    std::cout << "|time: " << m_timeStamp << "\n";
    std::cout << "|Nonce: " << m_nonce << "\n";
    std::cout << "|Hash: " << cryptor.sha256HashToString(m_hash) << "\n";
    std::cout << "|PrevHash: " << cryptor.sha256HashToString(m_prevBlockHash) << "\n";

    for(auto& tx: m_tx) {
        tx.print();
    }
    std::cout << std::setfill('=') << std::setw(80) << "\n" << std::endl;
}

void printBigInt(uint32_t* bigint) {
    for(int i = 0; i < 8; i++) {
        std::cout << std::setfill('0') << std::setw(8) << std::hex << bigint[i];
    }
    std::cout << "\n";
}

static int bigIntCmp(const std::array<uint8_t, 32>& left, const std::array<uint8_t, 32>& right) {
    for(int i = 0; i < 32; i++) {
        if(left[i] > right[i]) {
            return 1;
        } else if(left[i] < right[i]) {
            return -1;
        }
    }
    return 0;
}

ProofOfWork::ProofOfWork(Block* block) {
    m_nonce = 0;
    m_block = block;
    m_target[0] = 0;
    m_target[1] = 1 << 0;
    m_target[2] = 0;
    m_target[3] = 0;
}

std::string ProofOfWork::PrepareData() {
    std::string data;
    CryptoppImpl cryptor;
    data += std::to_string(m_block->getTimeStamp());

    for(auto& tx: m_block->m_tx) {
        data += tx.toString();
    }
    data += cryptor.sha256HashToString(m_block->getPrevBlockHash());
    data += std::to_string(m_nonce);
    return data;
}

void ProofOfWork::Run() {
    m_nonce = 0;
    std::array<uint8_t, 32> hash;
    CryptoppImpl Crypto;
    std::cout << "==============Block Hashing==============\n";
    while(m_nonce < MAXNONCE) {
        std::string data = PrepareData();

        hash = Crypto.sha256Hash(data);

        if(m_nonce % 100000 == 0) {
            std::cout << ">";
        }

        if(bigIntCmp(hash, m_target) == -1) {
            m_block->setNonce(m_nonce);
            m_block->m_hash = hash;
            break;
        }
        m_nonce += 1;
    }

    std::cout << "\n" << std::endl;
}
