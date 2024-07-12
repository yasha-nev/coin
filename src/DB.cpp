#include "DB.hpp"

DB::DB(){
    m_db = nullptr;
}

DB::~DB(){
    if (m_db){
        delete m_db;
    }
}

void DB::connect(){
    leveldb::Options options;
    leveldb::DB::Open(options, DBPATH, &m_db);
}

void DB::connectIfexist(){
    leveldb::Options options;
    options.create_if_missing = true;
    leveldb::DB::Open(options, DBPATH, &m_db);
}

std::string DB::getCurrentHash(){
    std::string hash;
    m_db->Get(leveldb::ReadOptions(), "l", &hash);
    return hash;
}

uint64_t DB::getCurrentId(const std::array<uint32_t, 8> &hash){
    uint64_t id;
    auto block = getBlockByHash(hash);
    id = block->getTransaction().back().m_id;
    return id;
}

std::unique_ptr<Block> DB::getBlockByHash(const std::array<uint32_t, 8> &hash){
    std::string byteBlock;
    
    leveldb::Slice key((char *) hash.data(), hash.size());
    m_db->Get(leveldb::ReadOptions(), key, &byteBlock);
    
    Block *block = new Block;
    
    block->decode((uint8_t *)byteBlock.c_str());
    
    return std::make_unique<Block>(block);
}

void DB::putBlock(const std::unique_ptr<Block> &block){
    size_t blockSize = 0;
    
    uint8_t encBlock[block->size()];
    
    block->encode(encBlock);
    
    leveldb::Slice key((char *) block->getHash().data(), block->getHash().size());
    
    leveldb::Slice value((char *)encBlock, block->size());
    leveldb::Slice value_hash((char *)block->getHash().data(), block->getHash().size());
    
    m_db->Put(leveldb::WriteOptions(), key, value);
    m_db->Put(leveldb::WriteOptions(), "l", value_hash);
}
