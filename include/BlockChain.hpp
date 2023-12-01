#ifndef BlockChain_hpp
#define BlockChain_hpp

#include "DB.hpp"
#include "Block.hpp"
#include <list>

using namespace std;

class BlockChain{
public:
    BlockChain();
    ~BlockChain();
    uint64_t getBalance(string user);
    void addBlock(string from, string to, int value);
    void printChain();
    Block *getBlock(uint32_t *hash);
    
private:
    list<TXInput> createInputs(string from, int valuem, int *rest);
    Block *newBlock(Transaction *tx, uint32_t *prevHashBlock);
    Block *genesisBlock();
    uint32_t m_cur_hash[8];
    uint64_t m_blocksIDs;
    DB m_db;
};

#endif
