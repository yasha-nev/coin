#include "Block.hpp"
#include "ByteReader.hpp"
#include "ByteWriter.hpp"
#include "net/Message.hpp"

#include <array>
#include <assert.h>
#include <iostream>
#include <list>
#include <memory>

void getBlockMsgTest() {
    std::list<Hash> hashes;
    hashes.push_back(Hash(std::vector<uint8_t> { 2, 5, 1, 2, 5 }));
    hashes.push_back(Hash(std::vector<uint8_t> { 4, 1, 5, 15, 25 }));

    GetBlocksMsg gblochMsg = GetBlocksMsg(hashes);
    GetBlocksMsg gblochMsg2 = GetBlocksMsg();

    ByteWriter byteWriter;
    gblochMsg.encode(byteWriter);
    const auto& bytes = byteWriter.bytes();
    ByteReader byteReader(as_bytes(bytes.data(), bytes.size()));

    gblochMsg2.decode(byteReader);

    assert(gblochMsg.getClientId() == gblochMsg2.getClientId());
    assert(gblochMsg.getCommand() == gblochMsg2.getCommand());
    assert(gblochMsg.getHashes() == gblochMsg2.getHashes());
}

void invMsgTest() {
    std::list<Hash> hashes;
    hashes.push_back(Hash(std::vector<uint8_t> { 2, 5, 1, 2, 5 }));
    hashes.push_back(Hash(std::vector<uint8_t> { 4, 1, 5, 15, 25 }));

    InvMsg invMsg = InvMsg(InvTypes::iBlock, hashes);
    InvMsg invMsg2 = InvMsg();

    ByteWriter byteWriter;
    invMsg.encode(byteWriter);
    const auto& bytes = byteWriter.bytes();
    ByteReader byteReader(as_bytes(bytes.data(), bytes.size()));

    invMsg2.decode(byteReader);

    assert(invMsg.getClientId() == invMsg2.getClientId());
    assert(invMsg.getCommand() == invMsg2.getCommand());
    assert(invMsg.getHashes() == invMsg2.getHashes());
}

void getDataMsgTest() {
    std::list<Hash> hashes;
    hashes.push_back(Hash(std::vector<uint8_t> { 2, 5, 1, 2, 5 }));
    hashes.push_back(Hash(std::vector<uint8_t> { 4, 1, 5, 15, 25 }));

    GetDataMsg getDataMsg = GetDataMsg(DataTypes::dBlock, hashes);
    GetDataMsg getDataMsg2 = GetDataMsg();

    ByteWriter byteWriter;
    getDataMsg.encode(byteWriter);
    const auto& bytes = byteWriter.bytes();
    ByteReader byteReader(as_bytes(bytes.data(), bytes.size()));

    getDataMsg2.decode(byteReader);

    assert(getDataMsg.getClientId() == getDataMsg2.getClientId());
    assert(getDataMsg.getCommand() == getDataMsg2.getCommand());
    assert(getDataMsg.getHashes() == getDataMsg2.getHashes());
}

Block createBlock() {
    Hash zero_hash = createZeroHash();
    std::list<Transaction> txs;
    uint64_t id = 0;
    std::string address = "address";
    txs.push_back(TransactionFactory::createCoinBase(id, address));
    auto block = Block(static_cast<uint64_t>(0), txs, zero_hash, Hash(), 0);
    ProofOfWork pow(block);
    pow.Run();

    return block;
}

void BlockMsgTest() {
    Block block1 = createBlock();
    BlockMsg blockMsg = BlockMsg(block1);
    BlockMsg blockMsg2 = BlockMsg();

    ByteWriter byteWriter;
    blockMsg.encode(byteWriter);
    const auto& bytes = byteWriter.bytes();
    ByteReader byteReader(as_bytes(bytes.data(), bytes.size()));

    blockMsg2.decode(byteReader);

    assert(blockMsg.getClientId() == blockMsg2.getClientId());
    assert(blockMsg.getCommand() == blockMsg2.getCommand());

    Block block2 = blockMsg2.getBlock();

    assert(block1.getTimeStamp() == block2.getTimeStamp());
    assert(block1.getNonce() == block2.getNonce());
    assert(block1.getHash() == block2.getHash());
    assert(block1.getPrevBlockHash() == block2.getPrevBlockHash());
}

Transaction createTransaction() {

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

    return TransactionFactory::createSimple(id, from, to, 0, inputs, 0);
}

void txMsgTest() {

    Transaction tx1 = createTransaction();

    TxMsg txMsg = TxMsg(tx1);
    TxMsg txMsg2 = TxMsg();

    ByteWriter byteWriter;
    txMsg.encode(byteWriter);
    const auto& bytes = byteWriter.bytes();
    ByteReader byteReader(as_bytes(bytes.data(), bytes.size()));

    txMsg2.decode(byteReader);

    assert(txMsg.getClientId() == txMsg2.getClientId());
    assert(txMsg.getCommand() == txMsg2.getCommand());
    assert(txMsg.getTransaction() == txMsg2.getTransaction());
}

void noFoundMsgTest() {
    NoFoundMsg noFoundMsg = NoFoundMsg();
    NoFoundMsg noFoundMsg2 = NoFoundMsg();

    ByteWriter byteWriter;
    noFoundMsg.encode(byteWriter);
    const auto& bytes = byteWriter.bytes();
    ByteReader byteReader(as_bytes(bytes.data(), bytes.size()));

    noFoundMsg2.decode(byteReader);

    assert(noFoundMsg.getClientId() == noFoundMsg2.getClientId());
    assert(noFoundMsg.getCommand() == noFoundMsg2.getCommand());
}

int main() {
    getBlockMsgTest();

    invMsgTest();

    getDataMsgTest();

    BlockMsgTest();

    txMsgTest();

    noFoundMsgTest();

    return 0;
}
