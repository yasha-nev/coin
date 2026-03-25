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
    uint64_t id;
    auto block = getBlockByHash(hash);
    id = block->getTransaction().back().m_id;
    return id;
}

std::unique_ptr<Block> LeveldbDataBase::getBlockByHash(const std::array<uint8_t, 32>& hash) {
    std::string byteBlock;

    leveldb::Slice key((char*) hash.data(), hash.size());
    m_db->Get(leveldb::ReadOptions(), key, &byteBlock);

    auto block = std::make_unique<Block>();
    block->decode((uint8_t*)byteBlock.c_str());

    return std::move(block);
}

void LeveldbDataBase::putBlock(const std::unique_ptr<Block>& block) {
    std::vector<uint8_t> encBlock(block->size());

    block->encode(encBlock.data());

    leveldb::Slice key((char*) block->getHash().data(), block->getHash().size());

    leveldb::Slice value((char*) encBlock.data(), block->size());
    leveldb::Slice value_hash(
        (char*) block->getHash().data(), block->getHash().size());

    m_db->Put(leveldb::WriteOptions(), key, value);
    m_db->Put(leveldb::WriteOptions(), "l", value_hash);
}
