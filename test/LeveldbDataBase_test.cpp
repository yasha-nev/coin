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
    std::array<uint8_t, 32> zero_hash = {0};
    std::list<Transaction> txs;
    uint64_t id = 0;
    std::string address = "address";
    txs.push_back(CoinBaseTransaction(id, address));
    auto block = std::make_unique<Block>(static_cast<uint64_t>(0), txs, zero_hash, std::array<uint8_t, 32>(), 0);
    ProofOfWork pow(block.get());
    pow.Run();
    
    return block;
}

static bool blocksCompire(std::unique_ptr<Block> &block1, std::unique_ptr<Block> &block2){
    bool flag = true;
    
    flag &= (block1->getHash() == block2->getHash());
    flag &= (block1->getNonce() == block2->getNonce());
    flag &= (block1->getPrevBlockHash() == block2->getPrevBlockHash());
    flag &= (block1->getTimeStamp() == block2->getTimeStamp());
    
    flag &= (block1->getTransactions() == block2->getTransactions());
    
    return flag;
}


int main(){
    LeveldbDataBase db = LeveldbDataBase();
    
    if (file_exist(DBPATH)){
        std::remove(DBPATH);
    }

    db.connect();

    auto block = createBlock();

    db.putBlock(block);

    std::array<uint8_t, 32> curhash = db.getCurrentHash();
    
    assert(curhash == block->getHash());
    
    auto block2 = db.getBlockByHash(curhash);
    
    assert(blocksCompire(block, block2));
    
    assert(0 == db.getCurrentId(curhash));
}
