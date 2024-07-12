#include "Block.hpp"
#include "Transaction.hpp"
#include <ctime>
#include <list>
#include <assert.h>


Block createBlock(){
    std::array<uint32_t, 8> zero_hash = {0, 0, 0, 0, 0, 0, 0, 0};
    std::list<Transaction> txs;
    uint64_t id = 0;
    std::string address = "address";
    txs.push_back(CoinBaseTransaction(id, address));
    
    // generate simple block
    Block block(static_cast<uint64_t>(0), txs, zero_hash, std::array<uint32_t, 8>(), 0);
    
    // generate block hash
    ProofOfWork pow(&block);
    pow.Run();
    
    return block;
}

int main(){
    
    Block block1 = createBlock();
    
    assert(array2String(block1.getHash()) == "000050147f3c6ccf100e8c4fff3782f4c48efa983fc48449c0b70e3738a8846c");
    
    uint8_t enc[block1.size()];
    
    block1.encode(enc);
    Block block2;
    block2.decode(enc);
    
    assert(block1.getTimeStamp() == block2.getTimeStamp());
    assert(block1.getNonce() == block2.getNonce());
    assert(block1.getHash() == block2.getHash());
    assert(block1.getPrevBlockHash() == block2.getPrevBlockHash());
}
