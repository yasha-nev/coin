#ifndef Block_hpp
#define Block_hpp

#include <string>
#include <sstream>
#include <iomanip>
#include <ctype.h>
#include <string.h>
#include <inttypes.h>
#include <iostream>
#include "sha256.hpp"
#include "Transaction.hpp"

#define MAXNONCE 1000000

using namespace std;

class ProofOfWork{
public:
    ProofOfWork(struct Block *block);
    ~ProofOfWork();
    void Run();
    
private:
    string PrepareData();
    uint32_t *m_target;
    Block *m_block;
    uint64_t m_nonce;
};

class Block{
public:
    ~Block();
    Block();
    Block(int64_t timeStamp, Transaction *tx, uint32_t *prevBlockHash, uint32_t *hash, int64_t nonce);
    
    uint64_t getTimeStamp();
    Transaction *getTransaction();
    uint32_t*getPrevBlockHash();
    uint32_t*getHash();
    uint64_t  getNonce();
    
    void setTimeStamp(int64_t timeStamp);
    void setTransaction(Transaction *trans);
    void setNonce(uint64_t nonce);
    void setPrevBlockHash(uint32_t *hash);
    void setHash(uint32_t *hash);
    
    uint8_t *encode(size_t *blockSize);
    void print();
    
private:
    int64_t m_timeStamp;
    Transaction *m_tx;
    uint32_t *m_prevBlockHash;
    uint32_t *m_hash;
    uint64_t m_nonce;
    
    friend ProofOfWork;
    friend Block *decode(uint8_t *dec);
};

Block *decode(uint8_t *dec);
std::string array2String(uint32_t *arr);
void printBigInt(uint32_t *bigint);

#endif
