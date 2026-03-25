#include "Block.hpp"

#include <gtest/gtest.h>

class BlockTest: public ::testing::Test {
protected:
    Block createTestBlock() {
        Hash zero_hash = createZeroHash();
        std::list<Transaction> txs;
        txs.push_back(TransactionFactory::createCoinBase(0, "address"));

        Block block(0, txs, zero_hash, Hash(), 0);

        ProofOfWork pow(block);
        pow.Run();
        return block;
    }
};

TEST_F(BlockTest, ProofOfWorkValidation) {
    Block block = createTestBlock();
    auto cryptor = crypto::createCryptoppImpl();

    std::string hashStr = cryptor->sha256HashToString(block.getHash());

    EXPECT_EQ(hashStr, "0000a50fa6922fdf263171c43e8cb925e0af6a855195b3db29b404b1b7cb851c");

    EXPECT_TRUE(hashStr.substr(0, 4) == "0000");
}

TEST_F(BlockTest, SerializationCycle) {
    Block block1 = createTestBlock();

    ByteWriter writer;
    block1.encode(writer);

    const auto& bytes = writer.bytes();
    ByteReader reader(as_bytes(bytes.data(), bytes.size()));

    Block block2;
    block2.decode(reader);

    EXPECT_EQ(block1.getTimeStamp(), block2.getTimeStamp());
    EXPECT_EQ(block1.getNonce(), block2.getNonce());
    EXPECT_EQ(block1.getHash(), block2.getHash());
    EXPECT_EQ(block1.getPrevBlockHash(), block2.getPrevBlockHash());

    const auto& txs1 = block1.getTransactions();
    const auto& txs2 = block2.getTransactions();
    ASSERT_EQ(txs1.size(), txs2.size());
    EXPECT_EQ(txs1, txs2);
}

TEST_F(BlockTest, SettersAndGetters) {
    Block block;
    Hash dummyHash({ 1, 2, 3 });

    block.setTimeStamp(123456789);
    block.setNonce(999);
    block.setPrevBlockHash(dummyHash);

    EXPECT_EQ(block.getTimeStamp(), 123456789);
    EXPECT_EQ(block.getNonce(), 999);
    EXPECT_EQ(block.getPrevBlockHash(), dummyHash);
}
