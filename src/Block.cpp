#include "Block.hpp"

Block::Block(
    const int64_t& timeStamp,
    const std::list<Transaction>& txs,
    const std::array<uint8_t, 32>& prevBlockHash,
    const std::array<uint8_t, 32>& hash,
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

void Block::setTransaction(const std::list<Transaction>& txs) {
    m_tx = txs;
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

void Block::encode(ByteWriter& byteWriter) const {
    byteWriter.write<uint64_t>(m_timeStamp);
    byteWriter.write<size_t>(m_tx.size());

    for(const auto& tx: m_tx) {
        tx.encode(byteWriter);
    }

    byteWriter.write_bytes(as_bytes(m_prevBlockHash));
    byteWriter.write_bytes(as_bytes(m_hash));

    byteWriter.write<uint64_t>(m_nonce);
}

void convertVectorToArray(std::span<const std::byte> vec, std::array<uint8_t, 32>& arr) {
    if(vec.size() != 32) {
        throw std::runtime_error("Размер вектора должен быть равен 32 элементам");
    }

    for(size_t i = 0; i < 32; ++i) {
        arr[i] = static_cast<uint8_t>(vec[i]);
    }
}

void Block::decode(ByteReader& byteReader) {
    m_timeStamp = byteReader.read<uint64_t>();
    size_t txSize = byteReader.read<size_t>();

    for(int i = 0; i < txSize; i++) {
        Transaction tx;
        tx.decode(byteReader);
        m_tx.push_back(tx);
    }

    convertVectorToArray(byteReader.read_bytes(sizeof(uint8_t) * 32), m_prevBlockHash);
    convertVectorToArray(byteReader.read_bytes(sizeof(uint8_t) * 32), m_hash);

    m_nonce = byteReader.read<uint64_t>();
}

void Block::print() const noexcept {
    CryptoppImpl cryptor;
    std::cout << std::setfill('=') << std::setw(40) << "BLOCK" << std::setfill('=') << std::setw(40)
              << "\n";
    std::cout << "|time: " << m_timeStamp << "\n";
    std::cout << "|Nonce: " << m_nonce << "\n";
    std::cout << "|Hash: " << cryptor.sha256HashToString(m_hash) << "\n";
    std::cout << "|PrevHash: " << cryptor.sha256HashToString(m_prevBlockHash) << "\n";

    for(const auto& tx: m_tx) {
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
