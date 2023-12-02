#include "BlockChain.hpp"

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

void BlockChain::addBlock(const std::string &from, const std::string &to, int value){
    m_blocksIDs++;
    int rest;
    std::list<TXInput> inputs = createInputs(from, value, &rest);
    if (inputs.empty()){
        std::cout << "not money" << std::endl;
        return;
    }
    Transaction *tx = realTransaction(m_blocksIDs, from, to, value, inputs, rest);
    Block *block_n = newBlock(tx, m_cur_hash);
    
    m_db.putBlock(block_n);
    m_cur_hash = block_n->getHash();
    
    delete block_n;
    delete tx;
}

Block* BlockChain::genesisBlock(){
    std::array<uint32_t, 8> zero_hash = {0, 0, 0, 0, 0, 0, 0, 0};
    Transaction * coinBase = coinBaseTrans("BcVtjVsKHxRNbne4MKCBxaQbJyoQsZQ8JTb1m3mAygwkyCLFP6Jvmrrf ");
    Block *block_n = newBlock(coinBase, zero_hash);
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

Block *BlockChain::newBlock(Transaction *tx, const std::array<uint32_t, 8> &prevHashBlock){
    Block *block_n = new Block( static_cast<uint64_t>(std::time(nullptr)),
                               tx,
                               prevHashBlock,
                               std::array<uint32_t, 8>(),
                               0);
    
    ProofOfWork pow(block_n);
    pow.Run();
    return block_n;
}

Block *BlockChain::getBlock(const std::array<uint32_t, 8> &hash){
    return m_db.getBlockByHash(hash);
}

TXOutput *getOutputs(const std::string &from, int value, int *count){
    return nullptr;
}

uint64_t BlockChain::getBalance(const std::string &user){
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
            if (tx->m_out[i].m_pubkey == user){
                if (l < it && outIds[l].outIndex == i){
                    break;
                }
                
                sum += tx->m_out[i].m_value;
            }
        }
        
        for (int i = 0; i < tx->m_inCount; i++){
            if (tx->m_in[i].m_pubkey == user){
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

std::list<TXInput> BlockChain::createInputs(const std::string &from, int value, int *rest){
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
            if (tx->m_out[i].m_pubkey == from){
                if (l < it && outIds[l].outIndex == i){
                    break;
                }
                ls.push_back(TXInput(tx->m_id, i, from));
                sum += tx->m_out[i].m_value;
            }
        }
        
        for (int i = 0; i < tx->m_inCount; i++){
            if (tx->m_in[i].m_pubkey == from){
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
