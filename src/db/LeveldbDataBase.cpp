#include "LeveldbDataBase.hpp"

void LeveldbDataBase::connect() {
    leveldb::Options options;
    options.create_if_missing = true;

    leveldb::DB* rawDb = nullptr;
    leveldb::Status status = leveldb::DB::Open(options, DBPATH, &rawDb);
    if(!status.ok()) {
        throw std::runtime_error("Failed to open/create LevelDB: " + status.ToString());
    }

    m_db = std::unique_ptr<leveldb::DB>(rawDb);
}

std::array<uint8_t, 32> LeveldbDataBase::getCurrentHash() {
    std::array<uint8_t, 32> res;
    std::string hash;

    m_db->Get(leveldb::ReadOptions(), "l", &hash);

    uint8_t* ptr = (uint8_t*) hash.c_str();
    for(int i = 0; i < 32; i++) {
        res[i] = *ptr;
        ptr++;
    }

    return res;
}

uint64_t LeveldbDataBase::getCurrentId(const std::array<uint8_t, 32>& hash) {
    auto block = getBlockByHash(hash);

    const auto& transactions = block->getTransactions();

    if(transactions.empty()) {
        return -1;
    }

    const auto& lastTransaction = transactions.back();
    uint64_t id = lastTransaction.getId();
    return id;
}

std::optional<Block> LeveldbDataBase::getBlockByHash(const std::array<uint8_t, 32>& hash) {
    std::string byteBlock;

    leveldb::Slice key((char*) hash.data(), hash.size());
    m_db->Get(leveldb::ReadOptions(), key, &byteBlock);

    Block block = Block();

    ByteReader byteReader(as_bytes(byteBlock.data(), byteBlock.size()));
    block.decode(byteReader);

    return block;
}

void LeveldbDataBase::putBlock(const Block& block) {
    ByteWriter bytewriter;
    block.encode(bytewriter);

    const auto& bytes = bytewriter.bytes();

    leveldb::Slice key((char*) block.getHash().data(), block.getHash().size());

    leveldb::Slice value((char*) bytes.data(), bytes.size());
    leveldb::Slice value_hash((char*) block.getHash().data(), block.getHash().size());

    m_db->Put(leveldb::WriteOptions(), key, value);
    m_db->Put(leveldb::WriteOptions(), "l", value_hash);
}
