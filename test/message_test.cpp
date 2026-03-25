#include "Block.hpp"
#include "net/Message.hpp"
#include "serialization/ByteReader.hpp"
#include "serialization/ByteWriter.hpp"

#include <gtest/gtest.h>

#include <array>
#include <assert.h>
#include <iostream>
#include <list>
#include <memory>

class MessageTest: public ::testing::Test {
protected:
    template<typename T>
    void SerializationCycle(T& msgOut, T& msgIn) {
        ByteWriter writer;
        msgOut.encode(writer);

        const auto& bytes = writer.bytes();
        ByteReader reader(as_bytes(bytes.data(), bytes.size()));

        msgIn.decode(reader);
    }

    std::list<Hash> createTestHashes() {
        return { Hash(std::vector<uint8_t> { 2, 5, 1, 2, 5 }),
                 Hash(std::vector<uint8_t> { 4, 1, 5, 15, 25 }) };
    }
};

TEST_F(MessageTest, GetBlocksMessage) {
    auto hashes = createTestHashes();
    GetBlocksMsg out(hashes);

    GetBlocksMsg in;
    SerializationCycle(out, in);

    EXPECT_EQ(out.getCommand(), in.getCommand());
    EXPECT_EQ(out.getHashes(), in.getHashes());
}

TEST_F(MessageTest, InventoryMessage) {
    auto hashes = createTestHashes();
    InvMsg out(InvTypes::iBlock, hashes);

    InvMsg in;
    SerializationCycle(out, in);

    EXPECT_EQ(out.getCommand(), in.getCommand());
    EXPECT_EQ(out.getHashes(), in.getHashes());
}

TEST_F(MessageTest, GetDataMessage) {
    auto hashes = createTestHashes();
    GetDataMsg out(DataTypes::dBlock, hashes);

    GetDataMsg in;
    SerializationCycle(out, in);

    EXPECT_EQ(out.getHashes(), in.getHashes());
}

TEST_F(MessageTest, BlockMessage) {
    Hash zero_hash = createZeroHash();
    std::list<Transaction> txs;
    txs.push_back(TransactionFactory::createCoinBase(0, "miner_addr"));

    Block block1(0, txs, zero_hash, Hash(), 0);
    ProofOfWork pow(block1);
    pow.Run();

    BlockMsg out(block1);
    BlockMsg in;
    SerializationCycle(out, in);

    EXPECT_EQ(out.getCommand(), in.getCommand());

    Block decodedBlock = in.getBlock();
    EXPECT_EQ(block1.getHash(), decodedBlock.getHash());
    EXPECT_EQ(block1.getNonce(), decodedBlock.getNonce());
    EXPECT_EQ(block1.getTimeStamp(), decodedBlock.getTimeStamp());
}

TEST_F(MessageTest, TransactionMessage) {
    // Используем упрощенное создание транзакции
    std::list<TXInput> inputs = { TXInput(0, -1, "pk1") };
    Transaction tx = TransactionFactory::createSimple(5, "A", "B", 100, inputs, 0);

    TxMsg out(tx);
    TxMsg in;
    SerializationCycle(out, in);

    EXPECT_EQ(out.getTransaction(), in.getTransaction());
}

TEST_F(MessageTest, NotFoundMessage) {
    NoFoundMsg out;

    NoFoundMsg in;
    SerializationCycle(out, in);

    EXPECT_EQ(out.getCommand(), in.getCommand());
}
