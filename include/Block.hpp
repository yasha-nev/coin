#ifndef Block_hpp
#define Block_hpp

#include <string>
#include <sstream>
#include <iomanip>
#include <ctype.h>
#include <string.h>
#include <array>

#include <inttypes.h>
#include <iostream>
#include "sha256.hpp"
#include "Transaction.hpp"

#define MAXNONCE 1000000

class ProofOfWork{
public:
    ProofOfWork(class Block *block);
    
    void Run();
    
private:
    std::string PrepareData();
    
    std::array<uint32_t, 8> m_target;
    
    Block *m_block;
    
    uint64_t m_nonce;
};

class Block{
public:
    ~Block();
    
    Block();
    
    Block(const int64_t &timeStamp, Transaction *tx, const std::array<uint32_t, 8> &prevBlockHash, const std::array<uint32_t, 8> &hash, const int64_t &nonce);
    
    uint64_t getTimeStamp();
    
    Transaction *getTransaction();
    
    std::array<uint32_t, 8> getPrevBlockHash();
    
    std::array<uint32_t, 8> getHash();
    
    uint64_t getNonce();
    
    void setTimeStamp(const int64_t &timeStamp);
    
    void setTransaction(Transaction *trans);
    
    void setNonce(const uint64_t &nonce);
    
    void setPrevBlockHash(const std::array<uint32_t, 8> &hash);
    
    void setHash(const std::array<uint32_t, 8> &hash);
    
    size_t size();
    
    void print();
    
    uint8_t *encode(size_t *blockSize);
    
private:
    int64_t m_timeStamp;
    
    Transaction *m_tx;
    
    std::array<uint32_t, 8> m_prevBlockHash;
    
    std::array<uint32_t, 8> m_hash;
    
    uint64_t m_nonce;
    
    friend ProofOfWork;
    
    friend Block *decode(uint8_t *dec);
};

Block *decode(uint8_t *dec);

std::string array2String(const std::array<uint32_t, 8> &arr);

void printBigInt(uint32_t *bigint);

#endif
