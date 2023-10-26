#ifndef BlockChain_hpp
#define BlockChain_hpp

#include <leveldb/db.h>
#include "Block.hpp"

using namespace std;

class BlockChain{
public:
    BlockChain();
    ~BlockChain();
    TXOutput *getOutputs(string from, int value, int *count);
    void addBlock(string from, string to, int value);
    void printChain();
    Block *getBlock(uint32_t *hash);
    
private:
    Block *newBlock(Transaction *tx, uint32_t *prevHashBlock);
    Block *genesisBlock();
    uint32_t cur_hash[8];
    
    leveldb::DB* m_db;
};

#endif
