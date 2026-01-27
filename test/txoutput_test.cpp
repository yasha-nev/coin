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

    auto data = input1.encode();
    TXOutput input5;
    input5.decode(data);
    assert(input1 == input5);
}
