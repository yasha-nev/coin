#include "LeveldbDataBase.hpp"

void LeveldbDataBase::connect(){
    leveldb::Options options;
    options.create_if_missing = true;

    leveldb::DB* rawDb = nullptr;
    leveldb::Status status = leveldb::DB::Open(options, DBPATH, &rawDb);
    if (!status.ok()) {
        throw std::runtime_error("Failed to open/create LevelDB: " + status.ToString());
    }

    m_db = std::unique_ptr<leveldb::DB>(rawDb);
}

void LeveldbDataBase::connectIfexist() {
    leveldb::Options options;
    options.create_if_missing = false;

    leveldb::DB* rawDb = nullptr;
    leveldb::Status status = leveldb::DB::Open(options, DBPATH, &rawDb);
    if (!status.ok()) {
        throw std::runtime_error("Failed to open existing LevelDB: " + status.ToString());
    }

    m_db = std::unique_ptr<leveldb::DB>(rawDb);
}
std::array<uint32_t, 8> LeveldbDataBase::getCurrentHash(){
    std::array<uint32_t, 8> res;
    std::string hash;
    
    m_db->Get(leveldb::ReadOptions(), "l", &hash);
    
    uint32_t *ptr = (uint32_t *)hash.c_str();
    for (int i = 0; i < 8; i++){
        res[i] = *ptr;
        ptr++;
    }
    
    return res;
}

uint64_t LeveldbDataBase::getCurrentId(const std::array<uint32_t, 8> &hash){
    uint64_t id;
    auto block = getBlockByHash(hash);
    id = block->getTransaction().back().m_id;
    return id;
}

std::unique_ptr<Block> LeveldbDataBase::getBlockByHash(const std::array<uint32_t, 8> &hash){
    std::string byteBlock;
    
    leveldb::Slice key((char *) hash.data(), hash.size() * sizeof(uint32_t));
    m_db->Get(leveldb::ReadOptions(), key, &byteBlock);
    
    Block *block = new Block;
    
    block->decode((uint8_t *)byteBlock.c_str());
    
    return std::make_unique<Block>(block);
}

void LeveldbDataBase::putBlock(const std::unique_ptr<Block> &block){
    uint8_t encBlock[block->size()];
    
    block->encode(encBlock);
    
    leveldb::Slice key((char *) block->getHash().data(), block->getHash().size() * sizeof(uint32_t));
    
    leveldb::Slice value((char *)encBlock, block->size());
    leveldb::Slice value_hash((char *)block->getHash().data(), block->getHash().size() * sizeof(uint32_t));
    
    m_db->Put(leveldb::WriteOptions(), key, value);
    m_db->Put(leveldb::WriteOptions(), "l", value_hash);
}
