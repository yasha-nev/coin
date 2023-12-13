#ifndef BlockChain_hpp
#define BlockChain_hpp

#include "DB.hpp"
#include "Block.hpp"
#include <list>
#include <array>

class BlockChain{
public:
    BlockChain();
    
    void printChain();
    
    void putBlock(Block *block);
    
    void createBlock(uint64_t time, std::list<Transaction *> tx);
    
    uint64_t getPastTransactionId();
    
    uint64_t getBalance(const std::string &pubkey, const std::string &address);
    
    Block *getBlock(const std::array<uint32_t, 8> &hash);
    
    Block *getPastBlock();
    
    std::array<uint32_t, 8> getPastBlockHash();
    
    std::list<std::array<uint32_t, 8>> getHashesBefore(std::array<uint32_t, 8> curHash);
    
    std::list<TXInput> getInputs(const std::string &pubkey, const std::string &address, int value, int *rest);
    
private:
    
    Block *newBlock(uint64_t time, std::list<Transaction *> tx, const std::array<uint32_t, 8> &hash);
    
    Block *genesisBlock();
    
    std::array<uint32_t, 8> m_cur_hash;
    
    DB m_db;
};

bool file_exist (const std::string& path);

#endif
