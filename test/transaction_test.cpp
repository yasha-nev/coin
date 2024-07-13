#include "Transaction.hpp"
#include <list>
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
    Transaction *tx = new RealTransaction(id, from, to, 0, inputs, 0);
    
    return std::make_unique<Transaction>(tx);
}


bool transactionCompare(const std::unique_ptr<Transaction> &tx1, const std::unique_ptr<Transaction> &tx2){
    bool flag = true;
    
    flag &= (tx1->m_id == tx2->m_id);
    
    auto tx1_in_itr = tx1->m_in.begin();
    auto tx2_in_itr = tx2->m_in.begin();
    
    while (tx1_in_itr != tx1->m_in.end() && tx2_in_itr != tx2->m_in.end()){
        flag &= (tx1_in_itr->m_outIndex == tx2_in_itr->m_outIndex);
        flag &= (tx1_in_itr->m_sign == tx2_in_itr->m_sign);
        flag &= (tx1_in_itr->m_pubkey == tx2_in_itr->m_pubkey);
        flag &= (tx1_in_itr->m_pubkey == tx2_in_itr->m_pubkey);
        
        ++tx1_in_itr;
        ++tx2_in_itr;
    }
    
    if (tx1_in_itr != tx1->m_in.end() || tx2_in_itr != tx2->m_in.end()){
        return false;
    }
    
    auto tx1_out_itr = tx1->m_out.begin();
    auto tx2_out_itr = tx2->m_out.begin();
    
    while (tx1_out_itr != tx1->m_out.end() && tx2_out_itr != tx2->m_out.end()){
        flag &= (tx1_out_itr->m_address == tx2_out_itr->m_address);
        flag &= (tx1_out_itr->m_value == tx2_out_itr->m_value);
        
        ++tx1_out_itr;
        ++tx2_out_itr;
    }
    
    if (tx1_out_itr != tx1->m_out.end() || tx2_out_itr != tx2->m_out.end()){
        return false;
    }
    
    return flag;
}

int main(){
    
    uint64_t id = 0;
    std::string address = "to";
    
    auto tx0 = std::unique_ptr<Transaction>(new CoinBaseTransaction(id, address));
    
    auto tx1 = createTransaction();
    
    assert(transactionCompare(tx0, tx1) == false);
    
    uint8_t enc[tx1->size()];
    
    tx1->encode(enc);
    
    auto tx2 = std::make_unique<Transaction>(0, 0, 0);
    
    tx2->decode(enc);
    
    assert(transactionCompare(tx1, tx2) == true);
}

