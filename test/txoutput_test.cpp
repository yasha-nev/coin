#include "TXOutput.hpp"
#include "serialization/ByteReader.hpp"
#include "serialization/ByteWriter.hpp"

#include <gtest/gtest.h>

TEST(TXOutputTest, ConstructorAndGetters) {
    int expectedValue = 500;
    std::string expectedAddr = "bc1qgd6f8c...addr";

    TXOutput out(expectedValue, expectedAddr);

    EXPECT_EQ(out.getValue(), expectedValue);
    EXPECT_EQ(out.getAddress(), expectedAddr);
}

TEST(TXOutputTest, CopyOperations) {
    TXOutput original(100, "address_a");

    TXOutput copyConstructed(original);
    EXPECT_EQ(original, copyConstructed);

    TXOutput copyAssigned;
    copyAssigned = original;
    EXPECT_EQ(original, copyAssigned);
}

TEST(TXOutputTest, MoveOperations) {
    TXOutput source(200, "move_me");

    TXOutput movedTo(std::move(source));

    EXPECT_EQ(movedTo.getValue(), 200);
    EXPECT_EQ(movedTo.getAddress(), "move_me");

    TXOutput moveAssigned;
    TXOutput source2(300, "another_addr");
    moveAssigned = std::move(source2);

    EXPECT_EQ(moveAssigned.getValue(), 300);
    EXPECT_EQ(moveAssigned.getAddress(), "another_addr");
}

TEST(TXOutputTest, SerializationCycle) {
    TXOutput original(777, "target_wallet_address");

    ByteWriter writer;
    original.encode(writer);

    const auto& bytes = writer.bytes();

    EXPECT_EQ(original.size(), bytes.size());

    ByteReader reader(as_bytes(bytes.data(), bytes.size()));
    TXOutput decoded;
    decoded.decode(reader);

    EXPECT_EQ(original, decoded);
    EXPECT_EQ(decoded.getValue(), 777);
    EXPECT_EQ(decoded.getAddress(), "target_wallet_address");
}

TEST(TXOutputTest, EqualityOperator) {
    TXOutput out1(10, "addr1");
    TXOutput out2(10, "addr1");
    TXOutput out3(20, "addr1");
    TXOutput out4(10, "addr2");

    EXPECT_TRUE(out1 == out2);
    EXPECT_FALSE(out1 == out3);
    EXPECT_FALSE(out1 == out4);
}

TEST(TXOutputTest, BoundaryValues) {
    TXOutput zeroOut(0, "");
    EXPECT_EQ(zeroOut.getValue(), 0);
    EXPECT_TRUE(zeroOut.getAddress().empty());

    int maxVal = std::numeric_limits<int>::max();
    TXOutput maxOut(maxVal, "max_addr");
    EXPECT_EQ(maxOut.getValue(), maxVal);
}
