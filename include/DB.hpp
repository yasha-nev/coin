#ifndef DB_hpp
#define DB_hpp

#include "Block.hpp"
#include "leveldb/db.h"

//Users/yasha_nev/projects/BlockChain/BlockChain/testdb
#define DBPATH "testdb"

class DB {
public:
    DB();
    
    ~DB();
    
    void connect();
    
    void connectIfexist();
    
    void putBlock(Block *block);
    
    std::string getCurrentHash();
    
    uint64_t getCurrentId(const std::array<uint32_t, 8> &hash);
    
    Block *getBlockByHash(const std::array<uint32_t, 8> &hash);
    
    leveldb::DB* m_db;
};

#include <stdio.h>

#endif /* DB_hpp */
