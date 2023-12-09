#ifndef BlockChain_hpp
#define BlockChain_hpp

#include "DB.hpp"
#include "Block.hpp"
#include <list>

class BlockChain{
public:
    BlockChain();
    
    uint64_t getBalance(const std::string &user);
    
    void addBlock(const std::string &from, const std::string &to, int value);
    
    void printChain();
    
    void putBlock(Block *block);
    
    Block *getBlock(const std::array<uint32_t, 8> &hash);
    
    Block *getPastBlock();
    
    std::array<uint32_t, 8> getPastBlockHash();
    
    std::list<std::array<uint32_t, 8>> getHashesBefore(std::array<uint32_t, 8> curHash);
    
private:
    std::list<TXInput> createInputs(const std::string &from, int valuem, int *rest);
    
    Block *newBlock(uint64_t time, Transaction *tx, const std::array<uint32_t, 8> &hash);
    
    Block *genesisBlock();
    
    std::array<uint32_t, 8> m_cur_hash;
    
    uint64_t m_blocksIDs;
    
    DB m_db;
};

#endif
