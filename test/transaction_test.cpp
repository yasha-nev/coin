#include "Transaction.hpp"

#include <gtest/gtest.h>

#include <list>
#include <memory>

class TransactionTest: public ::testing::Test {
protected:
    Transaction createSampleTransaction() {
        std::list<TXInput> inputs;
        inputs.push_back(TXInput(0, -1, "pubkey_1"));
        inputs.push_back(TXInput(1, 50, "pubkey_2"));

        return TransactionFactory::createSimple(5, "from", "to", 100, inputs, 10);
    }
};

TEST_F(TransactionTest, SerializationCycle) {
    Transaction original = createSampleTransaction();

    ByteWriter writer;
    original.encode(writer);

    const auto& bytes = writer.bytes();
    ByteReader reader(as_bytes(bytes.data(), bytes.size()));

    Transaction decoded;
    decoded.decode(reader);

    EXPECT_EQ(original, decoded);

    EXPECT_EQ(decoded.getId(), 5);
    EXPECT_EQ(decoded.getInputs().size(), 2);
    EXPECT_EQ(decoded.getOutputs().size(), 2);
}

TEST_F(TransactionTest, CoinbaseFactory) {
    uint64_t id = 100;
    std::string minerAddr = "miner_address";

    Transaction cb = TransactionFactory::createCoinBase(id, minerAddr);

    cb.print();

    EXPECT_EQ(cb.getId(), id);
    EXPECT_TRUE(cb.getInputs().empty());
    ASSERT_FALSE(cb.getOutputs().empty());
    EXPECT_EQ(cb.getOutputs()[0].getValue(), REWARD);
    EXPECT_EQ(cb.getOutputs()[0].getAddress(), minerAddr);
}

TEST_F(TransactionTest, EqualityOperator) {
    Transaction tx1 = createSampleTransaction();
    Transaction tx2 = createSampleTransaction();

    EXPECT_EQ(tx1, tx2);

    Transaction tx3 = TransactionFactory::createCoinBase(999, "addr");
    EXPECT_FALSE(tx1 == tx3);
}

TEST_F(TransactionTest, SizeConsistency) {
    Transaction tx = createSampleTransaction();

    ByteWriter writer;
    tx.encode(writer);

    EXPECT_EQ(tx.size(), writer.bytes().size());
}

TEST_F(TransactionTest, ToStringNotEmpty) {
    Transaction tx = createSampleTransaction();
    std::string s = tx.toString();
    EXPECT_FALSE(s.empty());
    EXPECT_NE(s.find("from"), std::string::npos);
}
