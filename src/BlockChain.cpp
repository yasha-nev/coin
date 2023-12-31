#include "BlockChain.hpp"
#include <cstdint>

bool file_exist (const std::string& path) {
    if (FILE *file = fopen(path.c_str(), "r")) {
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
        
    }
    else{
        m_db.connectIfexist();
        Block *block = genesisBlock();
        m_cur_hash = block->getHash();
        
        m_db.putBlock(block);
        delete block;
    }
}

void BlockChain::createBlock(uint64_t time, std::list<Transaction *> tx){
    Block *block_n = newBlock(static_cast<uint64_t>(std::time(nullptr)), tx, m_cur_hash);
    
    m_db.putBlock(block_n);
    m_cur_hash = block_n->getHash();
    
    delete block_n;
}

Block* BlockChain::genesisBlock(){
    std::array<uint32_t, 8> zero_hash = {0, 0, 0, 0, 0, 0, 0, 0};
    std::list<Transaction *> lst;
    lst.push_back(coinBaseTrans(0, "BcVtjVsKHxRNbne4MKCBxaQbJyoQsZQ8JTb1m3mAygwkyCLFP6Jvmrrf"));
    
    Block *block_n = newBlock(0, lst, zero_hash);
    m_cur_hash = block_n->getHash();
    
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
    
    if (curHash == hash){
        return lst;
    }
    
    while (!checkFirstBlock(hash)){
        if (hash == curHash){
            break;
        }
        
        Block *block = getBlock(hash);
        
        if (!block){
            break;
        }
        
        lst.push_front(hash);
        
        hash = block->getPrevBlockHash();
        
        delete block;
    }
    
    return lst;
    
}

Block *BlockChain::newBlock(uint64_t time, std::list<Transaction *> tx, const std::array<uint32_t, 8> &prevHashBlock){
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

uint64_t BlockChain::getPastTransactionId(){
    Block *block = m_db.getBlockByHash(m_cur_hash);
    
    uint64_t id = block->m_tx.back()->m_id;
    delete block;
    
    return id;
}

TXOutput *getOutputs(const std::string &from, int value, int *count){
    return nullptr;
}

uint64_t BlockChain::getBalance(const std::string &pubkey, const std::string &address){
    int sum = 0;
    
    std::array<uint32_t, 8> hash;
    
    struct outId{
        uint64_t id;
        int outIndex;
    };
    
    std::vector<struct outId> outIds;

    hash = m_cur_hash;
    std::string blc;
    
    while (!checkFirstBlock(hash)){
        Block *block = getBlock(hash);
        hash = block->getPrevBlockHash();
        
        std::list<Transaction *>txList = block->getTransaction();
        
        for (Transaction * tx : txList){
            
            std::vector<outId> sup;
            
            for (size_t l = 0; l < outIds.size(); l++){
                sup.push_back(outIds[l]);
            }
            
            for (int i = 0; i < tx->m_outCount; i++){
                if (tx->m_out[i].m_address == address){
                    
                    bool flag = true;
                    
                    for (size_t k = 0; k < sup.size(); k++){
                        if (outIds[k].outIndex == i){
                            flag = false;
                        }
                    }
                    
                    if (flag){
                        sum += tx->m_out[i].m_value;
                    }
                }
            }
            
            for (int i = 0; i < tx->m_inCount; i++){
                if (tx->m_in[i].m_pubkey == pubkey){
                    
                    outId oi = {tx->m_in[i].m_tranId, tx->m_in[i].m_outIndex};
                    
                    outIds.push_back(oi);
                }
            }
        }
        
        delete block;
    }
    
    return sum;
}

std::list<TXInput> BlockChain::getInputs(const std::string &pubkey, const std::string &address, int value, int *rest){
    std::list<TXInput> ls;
    
    int sum = 0;
    
    std::array<uint32_t, 8> hash;
    
    *rest = 0;
    
    struct outId{
        uint64_t id;
        int outIndex;
    };
    
    std::vector<struct outId> outIds;

    hash = m_cur_hash;
    std::string blc;
    
    while (!checkFirstBlock(hash)){
        Block *block = getBlock(hash);
        hash = block->getPrevBlockHash();
        
        std::list<Transaction *>txList = block->getTransaction();
        
        for (Transaction * tx : txList){
            
            std::vector<outId> sup;
            
            for (size_t l = 0; l < outIds.size(); l++){
                sup.push_back(outIds[l]);
            }
            
            for (int i = 0; i < tx->m_outCount; i++){
                if (tx->m_out[i].m_address == address){
                    
                    bool flag = true;
                    
                    for (size_t k = 0; k < sup.size(); k++){
                        if (outIds[k].outIndex == i){
                            flag = false;
                        }
                    }
                    
                    if (flag){
                        ls.push_back(TXInput(tx->m_id, i, pubkey));
                        sum += tx->m_out[i].m_value;
                    }
                }
            }
            
            for (int i = 0; i < tx->m_inCount; i++){
                if (tx->m_in[i].m_pubkey == pubkey){
                    
                    outId oi = {tx->m_in[i].m_tranId, tx->m_in[i].m_outIndex};
                    
                    outIds.push_back(oi);
                }
            }
            
            if (sum >= value){
                break;
            }
        }
        
        delete block;
    }
    
    if (sum < value){
        ls.clear();
    }
    *rest = sum - value;
    
    return ls;
}
