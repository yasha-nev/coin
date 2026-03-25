#include "crypto/Hash.hpp"

#include <assert.h>
#include <iostream>

int main() {

    std::vector<uint8_t> testHash = { 0xAA, 0xBB, 0xCC, 0xDD };

    Hash originalHash(testHash);

    ByteWriter writer;
    originalHash.encode(writer);

    ByteReader reader(writer.bytes());
    Hash deserializedHash;
    deserializedHash.decode(reader);

    assert(originalHash == deserializedHash);
}
