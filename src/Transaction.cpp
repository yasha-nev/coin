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
        for (int i = 0; i < out_count; i++){
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
        s += sizeof(char) * in[i].pubkey.size();
    }
    
    s += sizeof(int);
    for (int i = 0; i < out_count; i++){
        s += sizeof(int);
        s += sizeof(size_t);
        s += sizeof(char) * out[i].pubkey.size();
    }
    
    return s;
}

void Transaction::encode(uint8_t *ptr){
    std::memcpy(ptr, &id, sizeof(uint64_t));
    ptr += sizeof(uint64_t);
    
    std::memcpy(ptr, &in_count, sizeof(int));
    ptr += sizeof(int);
    
    for (int i = 0; i < in_count; i++){
        std::memcpy(ptr, &in[i].tranId, sizeof(uint64_t));
        ptr += sizeof(uint64_t);
        
        std::memcpy(ptr, &in[i].outIndex, sizeof(int));
        ptr += sizeof(int);
        
        size_t pub_size = in[i].pubkey.size();
        std::memcpy(ptr, &pub_size, sizeof(size_t));
        ptr += sizeof(size_t);
        
        for (int j = 0; j < pub_size; j++){
            *ptr = in[i].pubkey[j];
            ptr += sizeof(char);
        }
    }
    
    std::memcpy(ptr, &out_count, sizeof(int));
    ptr += sizeof(int);
    
    for (int i = 0; i < out_count; i++){
        std::memcpy(ptr, &out[i].value, sizeof(int));
        ptr += sizeof(int);
        size_t pub_size = out[i].pubkey.size();
        
        std::memcpy(ptr, &pub_size, sizeof(size_t));
        ptr += sizeof(size_t);
        
        for (int j = 0; j < pub_size; j++){
            *ptr = out[i].pubkey[j];
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
        std::memcpy(&in[i].tranId, ptr, sizeof(uint64_t));
        ptr += sizeof(uint64_t);
        
        std::memcpy(&in[i].outIndex, ptr, sizeof(int));
        ptr += sizeof(int);
        
        size_t pub_size = 0;
        std::memcpy(&pub_size, ptr, sizeof(size_t));
        ptr += sizeof(size_t);
        
        for (int j = 0; j < pub_size; j++){
            in[i].pubkey += *ptr;
            ptr += sizeof(char);
        }
    }
    
    std::memcpy(&out_count, ptr, sizeof(int));
    ptr += sizeof(int);
    
    out = new TXOutput[out_count];
    for (int i = 0; i < out_count; i++){
        std::memcpy(&out[i].value, ptr, sizeof(int));
        ptr += sizeof(int);
        
        size_t pub_size = 0;
        std::memcpy(&pub_size, ptr, sizeof(size_t));
        ptr += sizeof(size_t);
        
        for (int j = 0; j < pub_size; j++){
            out[i].pubkey += *ptr;
            ptr += sizeof(char);
        }
    }
}

string Transaction::toString(){
    string result;
    result += to_string(id);
    result += to_string(in_count);
    for (int i = 0; i < in_count; i++){
        result += to_string(in[i].tranId);
        result += to_string(in[i].outIndex);
        result += in[i].pubkey;
    }
    result += to_string(out_count);
    for (int i = 0; i < out_count; i++){
        result += to_string(out[i].value);
        result += out[i].pubkey;
    }
    
    return result;
}

Transaction *coinBaseTrans(string pubkey){
    Transaction *TX = new Transaction(0, 0, 1);
    TX->id = 0;
    
    // Транзакция не ссылается ни на какие выходы
    // Отправителя нет
    
    
    /*TX->in_count = 1;
    TX->in->tranId = 0;
    TX->in->outIndex = 0;
    TX->in->pubkey += pubkey;*/
    
    //REWARD - награза 50бит
    //Кому - майнеру
    TX->out_count = 1;
    TX->out->value = REWARD;
    TX->out->pubkey += pubkey;
    
    return TX;
}

Transaction *simpleTrans(uint64_t id, string pubkey, int value){
    Transaction *TX = new Transaction(id, 0, 1);
    TX->id = id;
    TX->out_count = 1;
    TX->out->value = value;
    TX->out->pubkey += pubkey;
    
    return TX;
}

Transaction *realTransaction(uint64_t id, string from, string to, int value, list<TXInput> inputs, int rest){
    int out_count = 1;
    if (rest > 0){
        out_count++;
    }
    
    Transaction *TX = new Transaction(id, (int) inputs.size(), out_count);
    
    int i = 0;
    std::list<TXInput>::iterator it;
    for (it = inputs.begin(); it != inputs.end(); it++){
        TX->in[i].tranId = it->tranId;
        TX->in[i].outIndex = it->outIndex;
        TX->in[i].pubkey = it->pubkey;
        i++;
    }
    
    TX->out_count = out_count;
    TX->out[0].value = value;
    TX->out[0].pubkey += to;
    
    if (rest > 0){
        TX->out[1].value = rest;
        TX->out[1].pubkey += from;
    }
    
    return TX;
}
