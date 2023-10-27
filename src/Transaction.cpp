#include "Transaction.hpp"

#include <iostream>
using namespace std;

Transaction::Transaction(uint64_t id, int inCount, int outCount){
    m_id = id;
    m_inCount = inCount;
    m_outCount = outCount;
    
    if (m_inCount > 0){
        m_in = new struct TXInput[inCount];
    }
    
    if (m_outCount > 0){
        m_out = new struct TXOutput[m_outCount];
    }
}

Transaction::Transaction(Transaction *tx){
    m_id = tx->m_id;
    m_inCount = tx->m_inCount;
    m_outCount = tx->m_outCount;
    
    if (m_inCount > 0){
        m_in = new struct TXInput[m_inCount];
        for (int i = 0; i < m_inCount; i++){
            m_in[i] = tx->m_in[i];
        }
    }
    
    if (m_outCount > 0){
        m_out = new struct TXOutput[m_outCount];
        for (int i = 0; i < m_outCount; i++){
            m_out[i] = tx->m_out[i];
        }
    }
}

Transaction::~Transaction(){
    if (m_inCount > 0){
        delete[] m_in;
    }
    if (m_outCount > 0){
        delete[] m_out;
    }
}

size_t Transaction::size(){
    size_t s = 0;
    
    s += sizeof(uint64_t);
    
    s += sizeof(int);
    for (int i = 0; i < m_inCount; i++){
        s += sizeof(uint64_t);
        s += sizeof(int);
        s += sizeof(size_t);
        s += sizeof(char) * m_in[i].m_pubkey.size();
    }
    
    s += sizeof(int);
    for (int i = 0; i < m_outCount; i++){
        s += sizeof(int);
        s += sizeof(size_t);
        s += sizeof(char) * m_out[i].m_pubkey.size();
    }
    
    return s;
}

void Transaction::encode(uint8_t *ptr){
    std::memcpy(ptr, &m_id, sizeof(uint64_t));
    ptr += sizeof(uint64_t);
    
    std::memcpy(ptr, &m_inCount, sizeof(int));
    ptr += sizeof(int);
    
    for (int i = 0; i < m_inCount; i++){
        std::memcpy(ptr, &m_in[i].m_tranId, sizeof(uint64_t));
        ptr += sizeof(uint64_t);
        
        std::memcpy(ptr, &m_in[i].m_outIndex, sizeof(int));
        ptr += sizeof(int);
        
        size_t pubSize = m_in[i].m_pubkey.size();
        std::memcpy(ptr, &pubSize, sizeof(size_t));
        ptr += sizeof(size_t);
        
        for (int j = 0; j < pubSize; j++){
            *ptr = m_in[i].m_pubkey[j];
            ptr += sizeof(char);
        }
    }
    
    std::memcpy(ptr, &m_outCount, sizeof(int));
    ptr += sizeof(int);
    
    for (int i = 0; i < m_outCount; i++){
        std::memcpy(ptr, &m_out[i].m_value, sizeof(int));
        ptr += sizeof(int);
        size_t pubSize = m_out[i].m_pubkey.size();
        
        std::memcpy(ptr, &pubSize, sizeof(size_t));
        ptr += sizeof(size_t);
        
        for (int j = 0; j < pubSize; j++){
            *ptr = m_out[i].m_pubkey[j];
            ptr += sizeof(char);
        }
    }
}

void Transaction::decode(uint8_t *ptr){
    if (m_in){
        delete[] m_in;
    }
    if (m_out){
        delete[] m_out;
    }
    
    std::memcpy(&m_id, ptr, sizeof(uint64_t));
    ptr += sizeof(uint64_t);
    
    std::memcpy(&m_inCount, ptr, sizeof(int));
    ptr += sizeof(int);
    
    m_in = new TXInput[m_inCount];
    for (int i = 0; i < m_inCount; i++){
        std::memcpy(&m_in[i].m_tranId, ptr, sizeof(uint64_t));
        ptr += sizeof(uint64_t);
        
        std::memcpy(&m_in[i].m_outIndex, ptr, sizeof(int));
        ptr += sizeof(int);
        
        size_t pub_size = 0;
        std::memcpy(&pub_size, ptr, sizeof(size_t));
        ptr += sizeof(size_t);
        
        for (int j = 0; j < pub_size; j++){
            m_in[i].m_pubkey += *ptr;
            ptr += sizeof(char);
        }
    }
    
    std::memcpy(&m_outCount, ptr, sizeof(int));
    ptr += sizeof(int);
    
    m_out = new TXOutput[m_outCount];
    for (int i = 0; i < m_outCount; i++){
        std::memcpy(&m_out[i].m_value, ptr, sizeof(int));
        ptr += sizeof(int);
        
        size_t pub_size = 0;
        std::memcpy(&pub_size, ptr, sizeof(size_t));
        ptr += sizeof(size_t);
        
        for (int j = 0; j < pub_size; j++){
            m_out[i].m_pubkey += *ptr;
            ptr += sizeof(char);
        }
    }
}

string Transaction::toString(){
    string result;
    result += to_string(m_id);
    result += to_string(m_inCount);
    for (int i = 0; i < m_inCount; i++){
        result += to_string(m_in[i].m_tranId);
        result += to_string(m_in[i].m_outIndex);
        result += m_in[i].m_pubkey;
    }
    result += to_string(m_outCount);
    for (int i = 0; i < m_outCount; i++){
        result += to_string(m_out[i].m_value);
        result += m_out[i].m_pubkey;
    }
    
    return result;
}

Transaction *coinBaseTrans(string pubkey){
    Transaction *TX = new Transaction(0, 0, 1);
    TX->m_id = 0;
    
    // Транзакция не ссылается ни на какие выходы
    // Отправителя нет
    // REWARD - награза 50бит
    // Кому - майнеру
    
    TX->m_outCount = 1;
    *(TX->m_out) = TXOutput(REWARD, pubkey);
    return TX;
}

Transaction *simpleTrans(uint64_t id, string pubkey, int value){
    Transaction *TX = new Transaction(id, 0, 1);
    TX->m_id = id;
    TX->m_outCount = 1;
    
    *(TX->m_out) = TXOutput(value, pubkey);
    return TX;
}

Transaction *realTransaction(uint64_t id, string from, string to, int value, list<TXInput> inputs, int rest){
    int m_outCount = 1;
    if (rest > 0){
        m_outCount++;
    }
    
    Transaction *TX = new Transaction(id, (int) inputs.size(), m_outCount);
    
    int i = 0;
    std::list<TXInput>::iterator it;
    for (it = inputs.begin(); it != inputs.end(); it++){
        TX->m_in[i] = *it;
        i++;
    }
    
    TX->m_outCount = m_outCount;
    TX->m_out[0] = TXOutput(value, to);
    
    if (rest > 0){
        TX->m_out[1] = TXOutput(rest, from);
    }
    
    return TX;
}


TXOutput::TXOutput(int value, string pubkey){
    m_value = value;
    m_pubkey = pubkey;
}

TXOutput::TXOutput(const TXOutput &out){
    m_value = out.m_value;
    m_pubkey = out.m_pubkey;
}

TXOutput& TXOutput::operator =(const TXOutput &out){
    if (this == &out) {
        return *this;
    }
    m_value = out.m_value;
    m_pubkey = out.m_pubkey;
    return *this;
}

TXInput::TXInput(uint64_t transId, int outIndex, string pubkey){
    m_tranId = transId;
    m_outIndex = outIndex;
    m_pubkey = pubkey;
}

TXInput::TXInput(const TXInput &in){
    m_tranId = in.m_tranId;
    m_outIndex = in.m_outIndex;
    m_pubkey = in.m_pubkey;
}

TXInput& TXInput::operator =(const TXInput &in){
    if (this == &in) {
        return *this;
    }
    m_tranId = in.m_tranId;
    m_outIndex = in.m_outIndex;
    m_pubkey = in.m_pubkey;
    return *this;
}

