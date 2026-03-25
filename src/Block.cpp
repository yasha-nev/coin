#include "Block.hpp"

Block::Block(
    const int64_t& timeStamp,
    const std::list<Transaction>& txs,
    const Hash& prevBlockHash,
    const Hash& hash,
    const int64_t& nonce) {
    m_timeStamp = timeStamp;
    m_prevBlockHash = prevBlockHash;
    m_hash = hash;
    m_nonce = nonce;
    m_tx = txs;
}

Block::Block() {
    m_timeStamp = 0;
    m_nonce = 0;
}

Block::~Block() {
}

uint64_t Block::getTimeStamp() const noexcept {
    return m_timeStamp;
}

const std::list<Transaction>& Block::getTransactions() const noexcept {
    return m_tx;
}

const Hash& Block::getPrevBlockHash() const noexcept {
    return m_prevBlockHash;
}

const Hash& Block::getHash() const noexcept {
    return m_hash;
}

uint64_t Block::getNonce() const noexcept {
    return m_nonce;
}

void Block::setTimeStamp(const int64_t& timeStamp) noexcept {
    m_timeStamp = timeStamp;
}

void Block::setTransaction(const std::list<Transaction>& txs) {
    m_tx = txs;
}

void Block::setNonce(const uint64_t& nonce) noexcept {
    m_nonce = nonce;
}

void Block::setPrevBlockHash(const Hash& hash) noexcept {
    m_prevBlockHash = hash;
}

void Block::setHash(const Hash& hash) noexcept {
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

void Block::encode(ByteWriter& byteWriter) const {
    byteWriter.write<uint64_t>(m_timeStamp);
    byteWriter.write<size_t>(m_tx.size());

    for(const auto& tx: m_tx) {
        tx.encode(byteWriter);
    }

    m_prevBlockHash.encode(byteWriter);
    m_hash.encode(byteWriter);
    byteWriter.write<uint64_t>(m_nonce);
}

void Block::decode(ByteReader& byteReader) {
    m_timeStamp = byteReader.read<uint64_t>();
    size_t txSize = byteReader.read<size_t>();

    for(size_t i = 0; i < txSize; i++) {
        Transaction tx;
        tx.decode(byteReader);
        m_tx.push_back(tx);
    }

    m_prevBlockHash.decode(byteReader);
    m_hash.decode(byteReader);

    m_nonce = byteReader.read<uint64_t>();
}

void Block::print() const noexcept {
    auto hashEncoder = IHashEncoder::create();
    std::cout << std::setfill('=') << std::setw(40) << "BLOCK" << std::setfill('=') << std::setw(40)
              << "\n";
    std::cout << "|time: " << m_timeStamp << "\n";
    std::cout << "|Nonce: " << m_nonce << "\n";
    std::cout << "|Hash: " << hashEncoder->sha256HashToString(m_hash) << "\n";
    std::cout << "|PrevHash: " << hashEncoder->sha256HashToString(m_prevBlockHash) << "\n";

    for(const auto& tx: m_tx) {
        tx.print();
    }
    std::cout << std::setfill('=') << std::setw(80) << "\n" << std::endl;
}

ProofOfWork::ProofOfWork(Block& block):
    m_block(block) {
    m_nonce = 0;
}

std::string ProofOfWork::PrepareData() {
    std::string data;
    auto hashEncoder = IHashEncoder::create();
    data += std::to_string(m_block.getTimeStamp());

    for(const auto& tx: m_block.getTransactions()) {
        data += tx.toString();
    }
    data += hashEncoder->sha256HashToString(m_block.getPrevBlockHash());
    data += std::to_string(m_nonce);
    return data;
}

void ProofOfWork::Run() {
    m_nonce = 0;
    Hash hash;
    auto hashEncoder = IHashEncoder::create();
    std::cout << "==============Block Hashing==============\n";
    while(m_nonce < MAXNONCE) {
        std::string data = PrepareData();

        hash = hashEncoder->sha256Hash(data);

        if(m_nonce % 100000 == 0) {
            std::cout << ">";
        }

        if(hashIsValid(hash)) {
            m_block.setNonce(m_nonce);
            m_block.setHash(hash);
            break;
        }
        m_nonce += 1;
    }

    std::cout << "\n" << std::endl;
}

bool ProofOfWork::hashIsValid(const Hash& hash) {
    const auto& array = hash.getHashAsVector();

    for(size_t i = 0; i < 2; i++) {
        if(array[i] != 0) {
            return false;
        }
    }

    return true;
}
