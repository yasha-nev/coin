#ifndef BlockChain_hpp
#define BlockChain_hpp

#include "DB.hpp"
#include "Block.hpp"
#include <list>
#include <array>
#include "Wallet.hpp"

class Wallet;

class BlockChain{
public:
    BlockChain();
    
    uint64_t getBalance(Wallet &user);
    
    void addBlock(Wallet &from, const std::string &address, int value);
    
    void printChain();
    
    void putBlock(Block *block);
    
    Block *getBlock(const std::array<uint32_t, 8> &hash);
    
    Block *getPastBlock();
    
    std::array<uint32_t, 8> getPastBlockHash();
    
    std::list<std::array<uint32_t, 8>> getHashesBefore(std::array<uint32_t, 8> curHash);
    
private:
    std::list<TXInput> createInputs(Wallet &from, int value, int *rest);

    void transactionSign(Wallet &wal, Transaction *tx);
    
    Block *newBlock(uint64_t time, Transaction *tx, const std::array<uint32_t, 8> &hash);
    
    Block *genesisBlock();
    
    std::array<uint32_t, 8> m_cur_hash;
    
    uint64_t m_blocksIDs;
    
    DB m_db;
};

#endif
