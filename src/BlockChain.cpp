#include "BlockChain.hpp"
#include <cstdint>

static inline bool file_exist (const std::string& name) {
    if (FILE *file = fopen(name.c_str(), "r")) {
        fclose(file);
        return true;
    } else {
        return false;
    }
}

static bool checkFirstBlock(const std::array<uint32_t, 8> &hash){
    bool flag = true;
    
    for (int i = 0; i < 8; i++){
        if (hash[i] != 0) {
            flag = false;
        }
    }
    
    return flag;
}

BlockChain::BlockChain(){
    if (file_exist(DBPATH)){
        m_db.connect();
        std::string current_hash = m_db.getCurrentHash();
        
        uint32_t *ptr = (uint32_t *)current_hash.c_str();
        
        for (int i = 0; i < 8; i++){
            m_cur_hash[i] = *ptr;
            ptr++;
        }
        
        m_blocksIDs = m_db.getCurrentId(m_cur_hash);
    }
    else{
        m_db.connectIfexist();
        Block *block = genesisBlock();
        m_cur_hash = block->getHash();
        
        m_db.putBlock(block);
        m_blocksIDs = 0;
        delete block;
    }
}

void BlockChain::addBlock(Wallet &from, const std::string &address, int value){
    m_blocksIDs++;
    int rest;
    std::list<TXInput> inputs = createInputs(from, value, &rest);
    if (inputs.empty()){
        std::cout << "not money" << std::endl;
        return;
    }
    Transaction *tx = realTransaction(m_blocksIDs, from.getAddres(), address, value, inputs, rest);
    
    transactionSign(from, tx);

    Block *block_n = newBlock(static_cast<uint64_t>(std::time(nullptr)), tx, m_cur_hash);
    
    m_db.putBlock(block_n);
    m_cur_hash = block_n->getHash();
    
    delete block_n;
    delete tx;
}

Block* BlockChain::genesisBlock(){
    std::array<uint32_t, 8> zero_hash = {0, 0, 0, 0, 0, 0, 0, 0};
    Transaction * coinBase = coinBaseTrans("BcVtjVsKHxRNbne4MKCBxaQbJyoQsZQ8JTb1m3mAygwkyCLFP6Jvmrrf");
    Block *block_n = newBlock(0, coinBase, zero_hash);
    m_cur_hash = block_n->getHash();
    delete coinBase;
    return block_n;
}

void BlockChain::printChain(){
    std::array<uint32_t, 8> hash;
    hash = m_cur_hash;
    std::string blc;
    
    while (!checkFirstBlock(hash)){
        Block *block = getBlock(hash);
        hash = block->getPrevBlockHash();
        block->print();
        delete block;
    }
}

std::list<std::array<uint32_t, 8>> BlockChain::getHashesBefore(std::array<uint32_t, 8> curHash){
    std::list<std::array<uint32_t, 8>> lst;
    
    std::array<uint32_t, 8> hash = m_cur_hash;
    std::string blc;
    
    while (!checkFirstBlock(hash)){
        if (hash == curHash){
            break;
        }
        Block *block = getBlock(hash);
        lst.push_front(hash);
        hash = block->getPrevBlockHash();
        delete block;
    }
    
    return lst;
    
}

Block *BlockChain::newBlock(uint64_t time, Transaction *tx, const std::array<uint32_t, 8> &prevHashBlock){
    Block *block_n = new Block(time,
                               tx,
                               prevHashBlock,
                               std::array<uint32_t, 8>(),
                               0);
    
    ProofOfWork pow(block_n);
    pow.Run();
    return block_n;
}

void BlockChain::putBlock(Block *block){
    if (!block){
        return;
    }
    
    m_db.putBlock(block);
    
    m_cur_hash = block->getHash();
}

Block *BlockChain::getBlock(const std::array<uint32_t, 8> &hash){
    return m_db.getBlockByHash(hash);
}

Block *BlockChain::getPastBlock(){
    return m_db.getBlockByHash(m_cur_hash);
}

std::array<uint32_t, 8> BlockChain::getPastBlockHash(){
    return m_cur_hash;
}

TXOutput *getOutputs(const std::string &from, int value, int *count){
    return nullptr;
}

uint64_t BlockChain::getBalance(Wallet &user){
    uint64_t sum = 0;
    std::array<uint32_t, 8> hash;
    
    struct outIds{
        uint64_t id;
        int outIndex;
    };
    
    struct outIds outIds[m_blocksIDs];
    size_t it = 0;
    
    hash = m_cur_hash;
    std::string blc;
    
    while (!checkFirstBlock(hash)){
        Block *block = getBlock(hash);
        hash = block->getPrevBlockHash();
        
        Transaction *tx = block->getTransaction();
        
        size_t l = 0;
        for (; l < it; l++){
            if (tx->m_id == outIds[l].id){
                break;
            }
        }
        
        for (int i = 0; i < tx->m_outCount; i++){
            if (tx->m_out[i].m_address == user.getAddres()){
                if (l < it && outIds[l].outIndex == i){
                    break;
                }
                
                sum += tx->m_out[i].m_value;
            }
        }
        
        for (int i = 0; i < tx->m_inCount; i++){
            if (tx->m_in[i].m_pubkey == user.getPubKey()->getKey()){
                outIds[it] = {tx->m_in[i].m_tranId, tx->m_in[i].m_outIndex};
                it++;
            }
        }
        
        delete block;
    }
    std::cout << "=================Balance=================\n";
    std::cout << "user: " << sum << "\n" << std::endl;
    
    return sum;
}

std::list<TXInput> BlockChain::createInputs(Wallet &from, int value, int *rest){
    std::list<TXInput> ls;
    int sum = 0;
    std::array<uint32_t, 8> hash;
    *rest = 0;
    
    
    struct outIds{
        uint64_t id;
        int outIndex;
    };
    
    struct outIds outIds[m_blocksIDs];
    size_t it = 0;

    hash = m_cur_hash;
    std::string blc;
    
    while (!checkFirstBlock(hash)){
        Block *block = getBlock(hash);
        hash = block->getPrevBlockHash();
        
        Transaction *tx = block->getTransaction();
        
        int l = 0;
        for (; l < it; l++){
            if (tx->m_id == outIds[l].id){
                break;
            }
        }
        
        for (int i = 0; i < tx->m_outCount; i++){
            if (tx->m_out[i].m_address == from.getAddres()){
                if (l < it && outIds[l].outIndex == i){
                    break;
                }
                ls.push_back(TXInput(tx->m_id, i, from.getPubKey()->getKey()));
                sum += tx->m_out[i].m_value;
            }
        }
        
        for (int i = 0; i < tx->m_inCount; i++){
            if (tx->m_in[i].m_pubkey == from.getPubKey()->getKey()){
                outIds[it] = {tx->m_in[i].m_tranId, tx->m_in[i].m_outIndex};
                it++;
            }
        }
        
        if (sum >= value){
            break;
        }
        
        delete block;
    }
    
    if (sum < value){
        ls.clear();
    }
    *rest = sum - value;
    
    return ls;
}

void BlockChain::transactionSign(Wallet &wal, Transaction *tx){
    std::string signstr;
    for (size_t i = 0; i < tx->m_outCount; i++){
        signstr += tx->m_out->m_address;
    }

	for (size_t i = 0; i < tx->m_inCount; i++){
        sha256 cryptor;
        std::string key = tx->m_in[i].m_pubkey + signstr;
        cryptor.Hash(key);
        tx->m_in[i].m_sign = cryptor.getHash();
    }
    return;
}
