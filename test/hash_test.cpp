#include "crypto/Hash.hpp"

#include <gtest/gtest.h>

TEST(HashTest, Constructors) {
    std::vector<uint8_t> data = { 0xDE, 0xAD, 0xBE, 0xEF };
    Hash h1(data);
    EXPECT_EQ(h1.getHashAsVector(), data);

    Hash h2(h1);
    EXPECT_EQ(h1, h2);

    Hash h3(std::move(h2));
    EXPECT_EQ(h3.getHashAsVector(), data);
}

TEST(HashTest, Serialization) {
    std::vector<uint8_t> testData = { 0xAA, 0xBB, 0xCC, 0xDD };
    Hash original(testData);

    ByteWriter writer;
    original.encode(writer);

    ByteReader reader(as_bytes(writer.bytes().data(), writer.bytes().size()));
    Hash deserialized;
    deserialized.decode(reader);

    EXPECT_EQ(original, deserialized);
    EXPECT_EQ(deserialized.getHashAsVector(), testData);
}

TEST(HashTest, ComparisonOperators) {
    Hash small({ 0x01 });
    Hash large({ 0x02 });
    Hash equal({ 0x01 });

    EXPECT_TRUE(small < large);
    EXPECT_TRUE(large > small);
    EXPECT_FALSE(small < equal);
    EXPECT_EQ(small, equal);
}

TEST(HashTest, ZeroHash) {
    Hash zero = createZeroHash();

    EXPECT_TRUE(checkZeroHash(zero));

    Hash nonZero({ 0x01 });
    EXPECT_FALSE(checkZeroHash(nonZero));
}

TEST(HashTest, Assignment) {
    Hash h1({ 0xAA });
    Hash h2;

    h2 = h1;
    EXPECT_EQ(h1, h2);

    Hash h3;
    h3 = std::move(h1);
    EXPECT_EQ(h3.getHashAsVector(), std::vector<uint8_t> { 0xAA });
}
