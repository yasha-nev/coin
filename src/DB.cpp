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
    Block *block = getBlockByHash(hash);
    id = block->getTransaction().back()->m_id;
    delete block;
    return id;
}

Block *DB::getBlockByHash(const std::array<uint32_t, 8> &hash){
    std::string byteBlock;
    
    leveldb::Slice key((char *) hash.data(), hash.size());
    m_db->Get(leveldb::ReadOptions(), key, &byteBlock);
    
    Block *block = decode((uint8_t *) byteBlock.c_str());
    
    return block;
}

void DB::putBlock(Block *block){
    size_t blockSize = 0;
    uint8_t *encBlock = block->encode(&blockSize);
    
    leveldb::Slice key((char *) block->getHash().data(), block->getHash().size());
    
    leveldb::Slice value((char *)encBlock, blockSize);
    leveldb::Slice value_hash((char *)block->getHash().data(), block->getHash().size());
    
    m_db->Put(leveldb::WriteOptions(), key, value);
    m_db->Put(leveldb::WriteOptions(), "l", value_hash);
    delete[] encBlock;
}
