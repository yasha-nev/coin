#include "LeveldbDataBase.hpp"
#include "Transaction.hpp"

#include <gtest/gtest.h>

#include <filesystem>

namespace fs = std::filesystem;

class LeveldbTest: public ::testing::Test {
protected:
    void SetUp() override {
        cleanDb();
        db.connect();
    }

    void TearDown() override {
        cleanDb();
    }

    void cleanDb() {
        if(fs::exists(DBPATH)) {
            fs::remove_all(DBPATH);
        }
    }

    Block createTestBlock() {
        Hash zero_hash = createZeroHash();
        std::list<Transaction> txs;
        txs.push_back(TransactionFactory::createCoinBase(0, "test_address"));

        Block block(static_cast<uint64_t>(0), txs, zero_hash, Hash(), 0);
        ProofOfWork pow(block);
        pow.Run();
        return block;
    }

    LeveldbDataBase db;
};

TEST_F(LeveldbTest, PutAndGetBlock) {
    Block originalBlock = createTestBlock();
    Hash blockHash = originalBlock.getHash();

    db.putBlock(originalBlock);

    auto curHash = db.getCurrentHash();
    ASSERT_TRUE(curHash.has_value());
    EXPECT_EQ(*curHash, blockHash);

    auto retrievedBlock = db.getBlockByHash(blockHash);
    ASSERT_TRUE(retrievedBlock.has_value());

    EXPECT_EQ(originalBlock.getHash(), retrievedBlock->getHash());
    EXPECT_EQ(originalBlock.getNonce(), retrievedBlock->getNonce());
    EXPECT_EQ(originalBlock.getTimeStamp(), retrievedBlock->getTimeStamp());
    EXPECT_EQ(originalBlock.getTransactions(), retrievedBlock->getTransactions());
}

TEST_F(LeveldbTest, GetCurrentId) {
    Block block = createTestBlock();
    db.putBlock(block);

    auto id = db.getCurrentId(block.getHash());
    ASSERT_TRUE(id.has_value());
    EXPECT_EQ(*id, 0);
}

TEST_F(LeveldbTest, GetNonExistentData) {
    Hash fakeHash = createZeroHash();

    auto block = db.getBlockByHash(fakeHash);
    EXPECT_FALSE(block.has_value());

    auto id = db.getCurrentId(fakeHash);
    EXPECT_FALSE(id.has_value());
}
