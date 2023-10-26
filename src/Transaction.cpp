#include "Transaction.hpp"

#include <iostream>
using namespace std;

Transaction::Transaction(uint64_t _id, int _count_in, int _count_out){
    id = _id;
    in_count = _count_in;
    out_count = _count_out;
    
    if (in_count > 0){
        in = new struct TXInput[in_count];
    }
    
    if (out_count > 0){
        out = new struct TXOutput[out_count];
    }
}

Transaction::Transaction(Transaction *tx){
    id = tx->id;
    in_count = tx->in_count;
    out_count = tx->out_count;
    
    if (in_count > 0){
        in = new struct TXInput[in_count];
        for (int i = 0; i < in_count; i++){
            in[i].outIndex = tx->in[i].outIndex;
            in[i].tranId = tx->in[i].tranId;
            in[i].pubkey = tx->in[i].pubkey;
        }
    }
    
    if (out_count > 0){
        out = new struct TXOutput[out_count];
        for (int i = 0; i < in_count; i++){
            out[i].value = tx->out[i].value;
            out[i].pubkey = tx->out[i].pubkey;
        }
    }
}

Transaction::~Transaction(){
    if (in_count > 0){
        delete[] in;
    }
    if (out_count > 0){
        delete[] out;
    }
}

size_t Transaction::size(){
    size_t s = 0;
    
    s += sizeof(uint64_t);
    
    s += sizeof(int);
    for (int i = 0; i < in_count; i++){
        s += sizeof(uint64_t);
        s += sizeof(int);
        s += sizeof(size_t);
        s += sizeof(char) * in->pubkey.size();
    }
    
    s += sizeof(int);
    for (int i = 0; i < out_count; i++){
        s += sizeof(int);
        s += sizeof(size_t);
        s += sizeof(char) * out->pubkey.size();
    }
    
    return s;
}

void Transaction::encode(uint8_t *ptr){
    std::memcpy(ptr, &id, sizeof(uint64_t));
    ptr += sizeof(uint64_t);
    
    std::memcpy(ptr, &in_count, sizeof(int));
    ptr += sizeof(int);
    
    for (int i = 0; i < in_count; i++){
        std::memcpy(ptr, &in->tranId, sizeof(uint64_t));
        ptr += sizeof(uint64_t);
        
        std::memcpy(ptr, &in->outIndex, sizeof(int));
        ptr += sizeof(int);
        
        size_t pub_size = in->pubkey.size();
        std::memcpy(ptr, &pub_size, sizeof(size_t));
        ptr += sizeof(size_t);
        
        for (int j = 0; j < pub_size; j++){
            *ptr = in->pubkey[j];
            ptr += sizeof(char);
        }
    }
    
    std::memcpy(ptr, &out_count, sizeof(int));
    ptr += sizeof(int);
    
    for (int i = 0; i < out_count; i++){
        std::memcpy(ptr, &out->value, sizeof(int));
        ptr += sizeof(int);
        size_t pub_size = out->pubkey.size();
        
        std::memcpy(ptr, &pub_size, sizeof(size_t));
        ptr += sizeof(size_t);
        
        for (int j = 0; j < pub_size; j++){
            *ptr = out->pubkey[j];
            ptr += sizeof(char);
        }
    }
}

void Transaction::decode(uint8_t *ptr){
    if (in){
        delete[] in;
    }
    if (out){
        delete[] out;
    }
    
    std::memcpy(&id, ptr, sizeof(uint64_t));
    ptr += sizeof(uint64_t);
    
    std::memcpy(&in_count, ptr, sizeof(int));
    ptr += sizeof(int);
    
    in = new TXInput[in_count];
    for (int i = 0; i < in_count; i++){
        std::memcpy(&in->tranId, ptr, sizeof(uint64_t));
        ptr += sizeof(uint64_t);
        
        std::memcpy(&in->outIndex, ptr, sizeof(int));
        ptr += sizeof(int);
        
        size_t pub_size = 0;
        std::memcpy(&pub_size, ptr, sizeof(size_t));
        ptr += sizeof(size_t);
        
        for (int j = 0; j < pub_size; j++){
            in->pubkey += *ptr;
            ptr += sizeof(char);
        }
    }
    
    std::memcpy(&out_count, ptr, sizeof(int));
    ptr += sizeof(int);
    
    out = new TXOutput[out_count];
    for (int i = 0; i < out_count; i++){
        std::memcpy(&out->value, ptr, sizeof(int));
        ptr += sizeof(int);
        
        size_t pub_size = 0;
        std::memcpy(&pub_size, ptr, sizeof(size_t));
        ptr += sizeof(size_t);
        
        for (int j = 0; j < pub_size; j++){
            out->pubkey += *ptr;
            ptr += sizeof(char);
        }
    }
    
    cout << "pubkeys: " << in->pubkey << " " << out->pubkey << "\n";
}

string Transaction::toString(){
    string result;
    result += to_string(id);
    result += to_string(in_count);
    for (int i = 0; i < in_count; i++){
        result += to_string(in->tranId);
        result += to_string(in->outIndex);
        result += in->pubkey;
    }
    result += to_string(out_count);
    for (int i = 0; i < out_count; i++){
        result += to_string(out->value);
        result += out->pubkey;
    }
    
    return result;
}

Transaction *coinBaseTrans(string pubkey){
    Transaction *TX = new Transaction(15, 1, 1);
    TX->id = 0;
    
    // Транзакция не ссылается ни на какие выходы
    // Отправителя нет
    
    
    TX->in_count = 1;
    TX->in->tranId = 0;
    TX->in->outIndex = 0;
    TX->in->pubkey += pubkey;
    
    //REWARD - награза 50бит
    //Кому - майнеру
    TX->out_count = 1;
    TX->out->value = REWARD;
    TX->out->pubkey += pubkey;
    
    return TX;
}
