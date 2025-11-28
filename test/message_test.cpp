#include "Block.hpp"
#include "Message.hpp"

#include <memory>
#include <list>
#include <array>
#include <assert.h>

void getBlockMsgTest() {
    std::list<std::array<uint8_t, 32>> hashes;

    hashes.push_back(std::array<uint8_t, 32>{1});
    hashes.push_back(std::array<uint8_t, 32>{0});

    auto gblochMsg = GetBlocksMsg(hashes);

    std::vector<uint8_t> encodeMsg = gblochMsg.toByte();
    GetBlocksMsg gblochMsg2 = GetBlocksMsg();
    gblochMsg2.parse(encodeMsg.data(), encodeMsg.size());

    assert(gblochMsg.getClientId() == gblochMsg2.getClientId());
    assert(gblochMsg.getCommand() == gblochMsg2.getCommand());
    assert(gblochMsg.getHashes() == gblochMsg2.getHashes());
}

void invMsgTest() {
    std::list<std::array<uint8_t, 32>> hashes;

    hashes.push_back(std::array<uint8_t, 32>{1});
    hashes.push_back(std::array<uint8_t, 32>{0});

    auto invMsg = InvMsg(InvTypes::iBlock, hashes);

    std::vector<uint8_t> encodeMsg = invMsg.toByte();
    InvMsg invMsg2 = InvMsg();
    invMsg2.parse(encodeMsg.data(), encodeMsg.size());

    assert(invMsg.getClientId() == invMsg2.getClientId());
    assert(invMsg.getCommand() == invMsg2.getCommand());
    assert(invMsg.getHashes() == invMsg2.getHashes());
}

void getDataMsgTest() {
    std::list<std::array<uint8_t, 32>> hashes;

    hashes.push_back(std::array<uint8_t, 32>{1});
    hashes.push_back(std::array<uint8_t, 32>{0});

    auto getDataMsg = GetDataMsg(DataTypes::dBlock, hashes);

    std::vector<uint8_t> encodeMsg = getDataMsg.toByte();
    GetDataMsg getDataMsg2 = GetDataMsg();
    getDataMsg2.parse(encodeMsg.data(), encodeMsg.size());

    assert(getDataMsg.getClientId() == getDataMsg2.getClientId());
    assert(getDataMsg.getCommand() == getDataMsg2.getCommand());
    assert(getDataMsg.getHashes() == getDataMsg2.getHashes());
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

void BlockMsgTest() {
    auto block1 = createBlock();

    auto blockMsg = BlockMsg(block1);

    std::vector<uint8_t> encodeMsg = blockMsg.toByte();
    BlockMsg blockMsg2 = BlockMsg();
    blockMsg2.parse(encodeMsg.data(), encodeMsg.size());

    assert(blockMsg.getClientId() == blockMsg2.getClientId());
    assert(blockMsg.getCommand() == blockMsg2.getCommand());

    auto block2 = blockMsg2.getBlock();

    assert(block1->getTimeStamp() == block2->getTimeStamp());
    assert(block1->getNonce() == block2->getNonce());
    assert(block1->getHash() == block2->getHash());
    assert(block1->getPrevBlockHash() == block2->getPrevBlockHash());
}

std::unique_ptr<Transaction> createTransaction(){
    
    std::list<TXInput> inputs;
    
    // =========== input1 ========= //
    
    uint64_t transid = 0;
    std::string pubkey = "pubkey_1";
    TXInput input_1 = TXInput(transid, -1, pubkey);
    inputs.push_back(input_1);
    
    // =========== input2 ========= //
    transid = 1;
    pubkey = "pubkey_2";
    TXInput input_2 = TXInput(transid, 50, pubkey);
    inputs.push_back(input_2);
    
    // =========== input3 ========= //
    transid = 1;
    pubkey = "pubkey_2";
    TXInput input_3 = TXInput(transid, 2, pubkey);
    inputs.push_back(input_3);
    
    // ======= Transaction ======== //
    uint64_t id = 5;
    std::string from = "from";
    std::string to = "to";
    
    return std::make_unique<RealTransaction>(id, from, to, 0, inputs, 0);
}

void txMsgTest() {

    auto tx1 = createTransaction();

    auto txMsg = TxMsg(tx1);

    std::vector<uint8_t> encodeMsg = txMsg.toByte();
    TxMsg txMsg2 = TxMsg();
    txMsg2.parse(encodeMsg.data(), encodeMsg.size());

    assert(txMsg.getClientId() == txMsg2.getClientId());
    assert(txMsg.getCommand() == txMsg2.getCommand());
    assert(txMsg.getTransaction() == txMsg2.getTransaction());
}

void noFoundMsgTest() {
    auto noFoundMsg = NoFoundMsg();

    std::vector<uint8_t> encodeMsg = noFoundMsg.toByte();
    NoFoundMsg noFoundMsg2 = NoFoundMsg();
    noFoundMsg2.parse(encodeMsg.data(), encodeMsg.size());

    assert(noFoundMsg.getClientId() == noFoundMsg2.getClientId());
    assert(noFoundMsg.getCommand() == noFoundMsg2.getCommand());
}


int main(){
    getBlockMsgTest();
    invMsgTest();
    getDataMsgTest();
    BlockMsgTest();
    txMsgTest();
    noFoundMsgTest();

    return 0;
}