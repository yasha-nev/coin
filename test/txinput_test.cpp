
#include "TXInput.hpp"
#include "serialization/ByteReader.hpp"
#include "serialization/ByteWriter.hpp"

#include <gtest/gtest.h>

class TXInputTest: public ::testing::Test {
protected:
    const uint64_t tId = 1234;
    const int outIdx = 1;
    const std::string pubkey = "test_signature";
    const std::string signature = "test_signature";
};

TEST_F(TXInputTest, EqualityAndCopyConstructors) {
    TXInput input1(tId, outIdx, pubkey);
    TXInput input2(tId, outIdx, pubkey);

    EXPECT_EQ(input1, input2);

    TXInput input3 = input1;
    EXPECT_EQ(input1, input3);

    TXInput input4(input1);
    EXPECT_EQ(input1, input4);
}

TEST_F(TXInputTest, SerializationCircle) {
    TXInput original(tId, outIdx, pubkey);
    original.setSignarure(signature);

    ByteWriter byteWriter;
    original.encode(byteWriter);

    const auto& bytes = byteWriter.bytes();
    ByteReader byteReader(as_bytes(bytes.data(), bytes.size()));

    TXInput decoded;
    decoded.decode(byteReader);

    EXPECT_EQ(original, decoded);
    EXPECT_EQ(decoded.getTransactionId(), tId);
    EXPECT_EQ(decoded.getOutIndex(), outIdx);
    EXPECT_EQ(decoded.getPublicKey(), pubkey);
    EXPECT_EQ(decoded.getSignature(), signature);
}

TEST_F(TXInputTest, GettersAndSetters) {
    TXInput input(tId, outIdx, pubkey);

    EXPECT_EQ(input.getTransactionId(), tId);
    EXPECT_EQ(input.getOutIndex(), outIdx);
    EXPECT_EQ(input.getPublicKey(), pubkey);

    input.setSignarure(signature);
    EXPECT_EQ(input.getSignature(), signature);
}
