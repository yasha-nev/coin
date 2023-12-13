#include "Transaction.hpp"

Transaction::Transaction(uint64_t id, int inCount, int outCount){
    m_id = id;
    m_inCount = inCount;
    m_outCount = outCount;
    
    if (m_inCount > 0){
        m_in = new class TXInput[inCount];
    }
    
    if (m_outCount > 0){
        m_out = new class TXOutput[m_outCount];
    }
}

Transaction::Transaction(Transaction *tx){
    m_id = tx->m_id;
    m_inCount = tx->m_inCount;
    m_outCount = tx->m_outCount;
    
    if (m_inCount > 0){
        m_in = new class TXInput[m_inCount];
        for (int i = 0; i < m_inCount; i++){
            m_in[i] = tx->m_in[i];
        }
    }
    
    if (m_outCount > 0){
        m_out = new class TXOutput[m_outCount];
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
        s += sizeof(size_t);
        s += sizeof(char) * m_in[i].m_sign.size();
    }
    
    s += sizeof(int);
    for (int i = 0; i < m_outCount; i++){
        s += sizeof(int);
        s += sizeof(size_t);
        s += sizeof(char) * m_out[i].m_address.size();
    }
    
    return s;
}

void Transaction::encode(uint8_t *ptr){
    memcpy(ptr, &m_id, sizeof(uint64_t));
    ptr += sizeof(uint64_t);
    
    memcpy(ptr, &m_inCount, sizeof(int));
    ptr += sizeof(int);
    
    for (int i = 0; i < m_inCount; i++){
        memcpy(ptr, &m_in[i].m_tranId, sizeof(uint64_t));
        ptr += sizeof(uint64_t);
        
        memcpy(ptr, &m_in[i].m_outIndex, sizeof(int));
        ptr += sizeof(int);
        
        size_t pubSize = m_in[i].m_pubkey.size();
        memcpy(ptr, &pubSize, sizeof(size_t));
        ptr += sizeof(size_t);
        
        for (size_t j = 0; j < pubSize; j++){
            *ptr = m_in[i].m_pubkey[j];
            ptr += sizeof(char);
        }

        size_t sigSize = m_in[i].m_sign.size();
        memcpy(ptr, &sigSize, sizeof(size_t));
        ptr += sizeof(size_t);

        for (size_t j = 0; j < sigSize; j++){
            *ptr = m_in[i].m_sign[j];
            ptr += sizeof(char);
        }
    }
    
    memcpy(ptr, &m_outCount, sizeof(int));
    ptr += sizeof(int);
    
    for (int i = 0; i < m_outCount; i++){
        memcpy(ptr, &m_out[i].m_value, sizeof(int));
        ptr += sizeof(int);
        size_t pubSize = m_out[i].m_address.size();
        
        memcpy(ptr, &pubSize, sizeof(size_t));
        ptr += sizeof(size_t);
        
        for (size_t j = 0; j < pubSize; j++){
            *ptr = m_out[i].m_address[j];
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
    
    memcpy(&m_id, ptr, sizeof(uint64_t));
    ptr += sizeof(uint64_t);
    
    memcpy(&m_inCount, ptr, sizeof(int));
    ptr += sizeof(int);
    
    m_in = new TXInput[m_inCount];
    for (int i = 0; i < m_inCount; i++){
        memcpy(&m_in[i].m_tranId, ptr, sizeof(uint64_t));
        ptr += sizeof(uint64_t);
        
        memcpy(&m_in[i].m_outIndex, ptr, sizeof(int));
        ptr += sizeof(int);
        
        size_t pubSize = 0;
        memcpy(&pubSize, ptr, sizeof(size_t));
        ptr += sizeof(size_t);
        
        for (size_t j = 0; j < pubSize; j++){
            m_in[i].m_pubkey += *ptr;
            ptr += sizeof(char);
        }

        size_t sigSize = 0;
        memcpy(&sigSize, ptr, sizeof(size_t));
        ptr += sizeof(size_t);
        
        for (size_t j = 0; j < sigSize; j++){
            m_in[i].m_sign += *ptr;
            ptr += sizeof(char);
        }
    }
    
    memcpy(&m_outCount, ptr, sizeof(int));
    ptr += sizeof(int);
    
    m_out = new TXOutput[m_outCount];
    for (int i = 0; i < m_outCount; i++){
        memcpy(&m_out[i].m_value, ptr, sizeof(int));
        ptr += sizeof(int);
        
        size_t pubSize = 0;
        memcpy(&pubSize, ptr, sizeof(size_t));
        ptr += sizeof(size_t);
        
        for (size_t j = 0; j < pubSize; j++){
            m_out[i].m_address += *ptr;
            ptr += sizeof(char);
        }
    }
}

std::string Transaction::toString(){
    std::string result;
    result += std::to_string(m_id);
    result += std::to_string(m_inCount);
    for (int i = 0; i < m_inCount; i++){
        result += std::to_string(m_in[i].m_tranId);
        result += std::to_string(m_in[i].m_outIndex);
        result += m_in[i].m_pubkey;
        result += m_in[i].m_sign;
    }
    result += std::to_string(m_outCount);
    for (int i = 0; i < m_outCount; i++){
        result += std::to_string(m_out[i].m_value);
        result += m_out[i].m_address;
    }
    
    return result;
}

void Transaction::print(){
    std::cout << "|transaction id: "  << m_id << "\n";
    std::cout << "|" << std::setfill('_') << std::setw(39) << "TXINPUTS" << std::setfill('_') << std::setw(40) << "\n";
    
    for (int i = 0; i < m_inCount; i++){
        std::cout << "|output id : "  << m_in[i].m_tranId << "\n";
        std::cout << "|index: "  <<  m_in[i].m_outIndex << "\n";
        std::cout << "|pubkey from: "  <<  m_in[i].m_pubkey << "\n";
        std::cout << "|sign: "  <<  m_in[i].m_sign << "\n";
        std::cout << "|" << std::setfill('-') << std::setw(79) << "\n";
    }
    std::cout << "|" << std::setfill('_') << std::setw(39) << "TXOUTPUTS" << std::setfill('_') << std::setw(40) << "\n";
    for (int i = 0; i < m_outCount; i++){
        std::cout << "|value: " <<  m_out[i].m_value << "\n";
        std::cout << "|address to: " <<  m_out[i].m_address << "\n";
        std::cout << "|" << std::setfill('-') << std::setw(79) << "\n";
    }
}

Transaction *coinBaseTrans(const uint64_t &id, const std::string &pubkey){
    Transaction *TX = new Transaction(0, 0, 1);
    TX->m_id = id;
    
    // Транзакция не ссылается ни на какие выходы
    // Отправителя нет
    // REWARD - награза 50бит
    // Кому - майнеру
    
    TX->m_outCount = 1;
    *(TX->m_out) = TXOutput(REWARD, pubkey);
    return TX;
}

Transaction *realTransaction(const uint64_t &id, const std::string &from, const std::string &to, int value, std::list<TXInput> inputs, int rest){
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


TXOutput::TXOutput(int value, const std::string &address){
    m_value = value;
    m_address = address;
}

TXOutput::TXOutput(const TXOutput &out){
    m_value = out.m_value;
    m_address = out.m_address;
}

TXOutput& TXOutput::operator =(const TXOutput &out){
    if (this == &out) {
        return *this;
    }
    m_value = out.m_value;
    m_address = out.m_address;
    return *this;
}

TXInput::TXInput(const uint64_t &transId, int outIndex, const std::string &pubkey){
    m_tranId = transId;
    m_outIndex = outIndex;
    m_pubkey = pubkey;
}

TXInput::TXInput(const TXInput &in){
    m_tranId = in.m_tranId;
    m_outIndex = in.m_outIndex;
    m_pubkey = in.m_pubkey;
    m_sign = in.m_sign;
}

TXInput& TXInput::operator =(const TXInput &in){
    if (this == &in) {
        return *this;
    }
    m_tranId = in.m_tranId;
    m_outIndex = in.m_outIndex;
    m_pubkey = in.m_pubkey;
    m_sign = in.m_sign;
    return *this;
}
