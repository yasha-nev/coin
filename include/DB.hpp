#ifndef DB_hpp
#define DB_hpp

#include "Block.hpp"
#include <leveldb/db.h>
#define DBPATH "/Users/yasha_nev/projects/BlockChain/BlockChain/testdb"

class DB {
public:
    DB();
    ~DB();
    void connect();
    void connectIfexist();
    void putBlock(Block *block);
    string getCurrentHash();
    uint64_t getCurrentId(uint32_t *hash);
    Block *getBlockByHash(uint32_t *hash);
    leveldb::DB* m_db;
};

#include <stdio.h>

#endif /* DB_hpp */
