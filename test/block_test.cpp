#include "Block.hpp"
#include "Transaction.hpp"
#include <ctime>
#include <list>
#include <assert.h>


Block createBlock(){
    std::array<uint8_t, 32> zero_hash = {0};
    std::list<Transaction> txs;
    uint64_t id = 0;
    std::string address = "address";
    txs.push_back(CoinBaseTransaction(id, address));
    
    // generate simple block
    Block block(static_cast<uint64_t>(0), txs, zero_hash, std::array<uint8_t, 32>(), 0);
    
    // generate block hash
    ProofOfWork pow(&block);
    pow.Run();
    
    return block;
}

int main(){
    CryptoppImpl cryptor;
    Block block1 = createBlock();

    assert(cryptor.sha256HashToString(block1.getHash()) == "0000a50fa6922fdf263171c43e8cb925e0af6a855195b3db29b404b1b7cb851c");
    
    std::vector<uint8_t> enc(block1.size());
    block1.encode(enc.data());
    Block block2;
    block2.decode(enc.data());
    
    assert(block1.getTimeStamp() == block2.getTimeStamp());
    assert(block1.getNonce() == block2.getNonce());
    assert(block1.getHash() == block2.getHash());
    assert(block1.getPrevBlockHash() == block2.getPrevBlockHash());
}
