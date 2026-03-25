
#include "ByteReader.hpp"
#include "ByteWriter.hpp"
#include "TXInput.hpp"

int main() {

    uint64_t transId = 0;
    std::string pubkey = "pubkey_1";
    TXInput input1 = TXInput(transId, -1, pubkey);
    TXInput input2 = TXInput(transId, -1, pubkey);
    assert(input1 == input2);

    TXInput input3 = input1;
    assert(input1 == input3);

    TXInput input4(input1);
    assert(input1 == input4);

    ByteWriter byteWriter;

    input1.encode(byteWriter);

    const auto& bytes = byteWriter.bytes();
    ByteReader byteReader(as_bytes(bytes.data(), bytes.size()));

    TXInput input5;
    input5.decode(byteReader);
    assert(input1 == input5);
}
