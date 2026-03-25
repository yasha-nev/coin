#include "TXOutput.hpp"

int main() {

    int value = 15;
    std::string address = "address";
    TXOutput input1 = TXOutput(0, address);
    TXOutput input2 = TXOutput(0, address);
    assert(input1 == input2);

    TXOutput input3 = input1;
    assert(input1 == input3);

    TXOutput input4(input1);
    assert(input1 == input4);

    ByteWriter byteWriter;
    input1.encode(byteWriter);

    const auto& bytes = byteWriter.bytes();
    ByteReader byteReader(as_bytes(bytes.data(), bytes.size()));

    TXOutput input5;
    input5.decode(byteReader);
    assert(input1 == input5);
}
