#include "Transaction.hpp"
#include "LeveldbDataBase.hpp"
#include <list>
#include <assert.h>
#include <fstream>
#include <iostream>

static bool file_exist (std::string path) {
    if (FILE *file = fopen(path.c_str(), "r")) {
        fclose(file);
        return true;
    } else {
        return false;
    }
}

std::unique_ptr<Block> createBlock(){
    std::array<uint32_t, 8> zero_hash = {0, 0, 0, 0, 0, 0, 0, 0};
    std::list<Transaction> txs;
    uint64_t id = 0;
    std::string address = "address";
    txs.push_back(CoinBaseTransaction(id, address));
    
    // generate simple block
    Block block = new Block(static_cast<uint64_t>(0), txs, zero_hash, std::array<uint32_t, 8>(), 0);
    
    // generate block hash
    ProofOfWork pow(&block);
    pow.Run();
    
    return std::make_unique<Block>(block);
}

static bool transactionCompare(std::list<Transaction> transaction1, std::list<Transaction> transaction2){
    bool flag = true;
    
    auto tx1 = transaction1.begin();
    auto tx2 = transaction2.begin();
    
    while (tx1 != transaction1.end() && tx2 != transaction2.end()){
        
        flag &= (tx1->m_id == tx2->m_id);
        
        auto tx1_in_itr = tx1->m_in.begin();
        auto tx2_in_itr = tx2->m_in.begin();
        
        while (tx1_in_itr != tx1->m_in.end() && tx2_in_itr != tx2->m_in.end()){
            flag &= (tx1_in_itr->m_outIndex == tx2_in_itr->m_outIndex);
            flag &= (tx1_in_itr->m_sign == tx2_in_itr->m_sign);
            flag &= (tx1_in_itr->m_pubkey == tx2_in_itr->m_pubkey);
            flag &= (tx1_in_itr->m_pubkey == tx2_in_itr->m_pubkey);
            
            ++tx1_in_itr;
            ++tx2_in_itr;
        }
        
        if (tx1_in_itr != tx1->m_in.end() || tx2_in_itr != tx2->m_in.end()){
            return false;
        }
        
        auto tx1_out_itr = tx1->m_out.begin();
        auto tx2_out_itr = tx2->m_out.begin();
        
        while (tx1_out_itr != tx1->m_out.end() && tx2_out_itr != tx2->m_out.end()){
            flag &= (tx1_out_itr->m_address == tx2_out_itr->m_address);
            flag &= (tx1_out_itr->m_value == tx2_out_itr->m_value);
            
            ++tx1_out_itr;
            ++tx2_out_itr;
        }
        
        if (tx1_out_itr != tx1->m_out.end() || tx2_out_itr != tx2->m_out.end()){
            return false;
        }
        
        ++tx1;
        ++tx2;
    }
    
    return flag;
}

static bool blocksCompire(std::unique_ptr<Block> &block1, std::unique_ptr<Block> &block2){
    bool flag = true;
    
    flag &= (block1->getHash() == block2->getHash());
    flag &= (block1->getNonce() == block2->getNonce());
    flag &= (block1->getPrevBlockHash() == block2->getPrevBlockHash());
    flag &= (block1->getTimeStamp() == block2->getTimeStamp());
    
    flag &= transactionCompare(block1->getTransaction(), block2->getTransaction());
    
    return flag;
}


int main(){
    LeveldbDataBase db = LeveldbDataBase();
    
    if (file_exist(DBPATH)){
        std::remove(DBPATH);
    }
    
    db.connectIfexist();
    
    auto block = createBlock();
    
    db.putBlock(block);
    
    std::array<uint32_t, 8> curhash = db.getCurrentHash();
    
    assert(curhash == block->getHash());
    
    auto block2 = db.getBlockByHash(curhash);
    
    assert(blocksCompire(block, block2));
    
    assert(0 == db.getCurrentId(curhash));
}
