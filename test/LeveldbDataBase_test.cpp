#include "LeveldbDataBase.hpp"
#include "Transaction.hpp"

#include <assert.h>
#include <fstream>
#include <iostream>
#include <list>

static bool file_exist(std::string path) {
    if(FILE* file = fopen(path.c_str(), "r")) {
        fclose(file);
        return true;
    } else {
        return false;
    }
}

Block createBlock() {
    Hash zero_hash = createZeroHash();
    std::list<Transaction> txs;
    uint64_t id = 0;
    std::string address = "address";
    txs.push_back(TransactionFactory::createCoinBase(id, address));
    Block block = Block(static_cast<uint64_t>(0), txs, zero_hash, Hash(), 0);
    ProofOfWork pow(block);
    pow.Run();

    return block;
}

static bool blocksCompire(Block& block1, Block& block2) {
    bool flag = true;

    flag &= (block1.getHash() == block2.getHash());
    flag &= (block1.getNonce() == block2.getNonce());
    flag &= (block1.getPrevBlockHash() == block2.getPrevBlockHash());
    flag &= (block1.getTimeStamp() == block2.getTimeStamp());
    flag &= (block1.getTransactions() == block2.getTransactions());

    return flag;
}

int main() {
    LeveldbDataBase db = LeveldbDataBase();

    if(file_exist(DBPATH)) {
        std::remove(DBPATH);
    }

    db.connect();

    Block block = createBlock();

    db.putBlock(block);

    std::optional<Hash> curhash = db.getCurrentHash();

    assert(curhash.has_value());

    assert(*curhash == block.getHash());

    std::optional<Block> block2 = db.getBlockByHash(*curhash);

    assert(block2.has_value());

    assert(blocksCompire(block, *block2));

    assert(0 == db.getCurrentId(*curhash));
}
