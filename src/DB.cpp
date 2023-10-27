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

string DB::getCurrentHash(){
    string hash;
    m_db->Get(leveldb::ReadOptions(), "l", &hash);
    return hash;
}

uint64_t DB::getCurrentId(uint32_t *hash){
    uint64_t id;
    Block *block = getBlockByHash(hash);
    id = block->getTransaction()->id;
    delete block;
    return id;
}

Block *DB::getBlockByHash(uint32_t *hash){
    string byteBlock;
    leveldb::Slice key((char *) hash, sizeof(uint32_t) * 8);
    m_db->Get(leveldb::ReadOptions(), key, &byteBlock);
    Block *block = decode((uint8_t *) byteBlock.c_str());
    return block;
}

void DB::putBlock(Block *block){
    size_t blockSize = 0;
    uint8_t *encBlock = block->encode(&blockSize);
    
    leveldb::Slice key((char *) block->getHash(), sizeof(uint32_t) * 8);
    leveldb::Slice value((char *)encBlock, blockSize);
    leveldb::Slice value_hash((char *)block->getHash(), sizeof(uint32_t) * 8);
    
    m_db->Put(leveldb::WriteOptions(), key, value);
    m_db->Put(leveldb::WriteOptions(), "l", value_hash);
    delete[] encBlock;
}
