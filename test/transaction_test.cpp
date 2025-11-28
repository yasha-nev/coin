#include "Transaction.hpp"
#include <list>
#include <memory>
#include <assert.h>

std::unique_ptr<Transaction> createTransaction(){
    
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
    
    return std::make_unique<RealTransaction>(id, from, to, 0, inputs, 0);
}

int main(){
    
    uint64_t id = 0;
    std::string address = "to";
    
    auto tx1 = createTransaction();

    std::vector<uint8_t> enc(tx1->size());
    
    tx1->encode(enc.data());
    
    auto tx2 = std::make_unique<Transaction>(0, 0, 0);
    
    tx2->decode(enc.data());
    
    assert(tx1 == tx2);
}

