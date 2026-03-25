#include "Transaction.hpp"

#include <assert.h>
#include <list>
#include <memory>

Transaction createTransaction() {

    std::list<TXInput> inputs;

    // =========== input1 ========= //

    uint64_t transid = 0;
    std::string pubkey = "pubkey_1";
    TXInput input_1 = TXInput(transid, -1, pubkey);
    inputs.push_back(input_1);

    // =========== input2 ========= //
    transid = 1;
    pubkey = "pubkey_2";
    TXInput input_2 = TXInput(transid, 50, pubkey);
    inputs.push_back(input_2);

    // =========== input3 ========= //
    transid = 1;
    pubkey = "pubkey_2";
    TXInput input_3 = TXInput(transid, 2, pubkey);
    inputs.push_back(input_3);

    // ======= Transaction ======== //
    uint64_t id = 5;
    std::string from = "from";
    std::string to = "to";

    return TransactionFactory::createSimple(id, from, to, 0, inputs, 0);
}

int main() {
    Transaction tx1 = createTransaction();

    ByteWriter byteWriter;
    tx1.encode(byteWriter);

    Transaction tx2 = Transaction();

    const auto& bytes = byteWriter.bytes();
    ByteReader byteReader(as_bytes(bytes.data(), bytes.size()));
    tx2.decode(byteReader);

    assert(tx1 == tx2);
}
