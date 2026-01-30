#include "Block.hpp"
#include "Transaction.hpp"

#include <assert.h>
#include <ctime>
#include <list>

Block createBlock() {
    Hash zero_hash = createZeroHash();
    std::list<Transaction> txs;
    uint64_t id = 0;
    std::string address = "address";
    txs.push_back(TransactionFactory::createCoinBase(id, address));

    // generate simple block
    Block block = Block(static_cast<uint64_t>(0), txs, zero_hash, Hash(), 0);

    // generate block hash
    ProofOfWork pow(block);
    pow.Run();

    return block;
}

int main() {
    CryptoppImpl cryptor;
    Block block1 = createBlock();

    assert(
        cryptor.sha256HashToString(block1.getHash()) ==
        "0000a50fa6922fdf263171c43e8cb925e0af6a855195b3db29b404b1b7cb851c");

    ByteWriter byteWriter;
    block1.encode(byteWriter);

    const auto& bytes = byteWriter.bytes();
    ByteReader byteReader(as_bytes(bytes.data(), bytes.size()));

    Block block2;
    block2.decode(byteReader);

    assert(block1.getTimeStamp() == block2.getTimeStamp());
    assert(block1.getNonce() == block2.getNonce());
    assert(block1.getHash() == block2.getHash());
    assert(block1.getPrevBlockHash() == block2.getPrevBlockHash());
    assert(block1.getTransactions() == block2.getTransactions());
}
