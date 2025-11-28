#include "Transaction.hpp"

Transaction::Transaction(uint64_t id, int inCount, int outCount) {
    m_id = id;
    m_in = std::vector<TXInput>(inCount);
    m_out = std::vector<TXOutput>(outCount);
}

Transaction::Transaction(Transaction* tx) {
    m_id = tx->m_id;
    m_in = tx->m_in;
    m_out = tx->m_out;
}

Transaction::~Transaction() {
}

size_t Transaction::size() const noexcept {
    size_t s = 0;

    s += sizeof(uint64_t);

    s += sizeof(int);
    for(int i = 0; i < m_in.size(); i++) {
        s += sizeof(uint64_t);
        s += sizeof(int);
        s += sizeof(size_t);
        s += sizeof(char) * m_in[i].m_pubkey.size();
        s += sizeof(size_t);
        s += sizeof(char) * m_in[i].m_sign.size();
    }

    s += sizeof(int);
    for(int i = 0; i < m_out.size(); i++) {
        s += sizeof(int);
        s += sizeof(size_t);
        s += sizeof(char) * m_out[i].m_address.size();
    }

    return s;
}

void Transaction::encode(uint8_t* ptr) {
    memcpy(ptr, &m_id, sizeof(uint64_t));
    ptr += sizeof(uint64_t);

    size_t inCount = m_in.size();

    memcpy(ptr, &inCount, sizeof(int));
    ptr += sizeof(int);

    for(size_t i = 0; i < m_in.size(); i++) {
        memcpy(ptr, &m_in[i].m_tranId, sizeof(uint64_t));
        ptr += sizeof(uint64_t);
        memcpy(ptr, &m_in[i].m_outIndex, sizeof(int));
        ptr += sizeof(int);

        size_t pubSize = m_in[i].m_pubkey.size();
        memcpy(ptr, &pubSize, sizeof(size_t));
        ptr += sizeof(size_t);

        for(size_t j = 0; j < pubSize; j++) {
            *ptr = m_in[i].m_pubkey[j];
            ptr += sizeof(char);
        }

        size_t sigSize = m_in[i].m_sign.size();
        memcpy(ptr, &sigSize, sizeof(size_t));
        ptr += sizeof(size_t);

        for(size_t j = 0; j < sigSize; j++) {
            *ptr = m_in[i].m_sign[j];
            ptr += sizeof(char);
        }
    }
    size_t outCount = m_out.size();
    memcpy(ptr, &outCount, sizeof(int));
    ptr += sizeof(int);

    for(size_t i = 0; i < m_out.size(); i++) {
        memcpy(ptr, &m_out[i].m_value, sizeof(int));
        ptr += sizeof(int);
        size_t pubSize = m_out[i].m_address.size();

        memcpy(ptr, &pubSize, sizeof(size_t));
        ptr += sizeof(size_t);

        for(size_t j = 0; j < pubSize; j++) {
            *ptr = m_out[i].m_address[j];
            ptr += sizeof(char);
        }
    }
}

void Transaction::decode(uint8_t* ptr) {
    m_in.clear();
    m_out.clear();

    memcpy(&m_id, ptr, sizeof(uint64_t));
    ptr += sizeof(uint64_t);

    size_t inCount = 0;

    memcpy(&inCount, ptr, sizeof(int));
    ptr += sizeof(int);

    m_in.resize(inCount);

    for(size_t i = 0; i < inCount; i++) {
        memcpy(&m_in[i].m_tranId, ptr, sizeof(uint64_t));
        ptr += sizeof(uint64_t);

        memcpy(&m_in[i].m_outIndex, ptr, sizeof(int));
        ptr += sizeof(int);

        size_t pubSize = 0;
        memcpy(&pubSize, ptr, sizeof(size_t));
        ptr += sizeof(size_t);

        for(size_t j = 0; j < pubSize; j++) {
            m_in[i].m_pubkey += *ptr;
            ptr += sizeof(char);
        }

        size_t sigSize = 0;
        memcpy(&sigSize, ptr, sizeof(size_t));
        ptr += sizeof(size_t);

        for(size_t j = 0; j < sigSize; j++) {
            m_in[i].m_sign += *ptr;
            ptr += sizeof(char);
        }
    }

    size_t outCount = 0;

    memcpy(&outCount, ptr, sizeof(int));
    ptr += sizeof(int);

    m_out.resize(outCount);
    for(size_t i = 0; i < outCount; i++) {
        memcpy(&m_out[i].m_value, ptr, sizeof(int));
        ptr += sizeof(int);

        size_t pubSize = 0;
        memcpy(&pubSize, ptr, sizeof(size_t));
        ptr += sizeof(size_t);

        for(size_t j = 0; j < pubSize; j++) {
            m_out[i].m_address += *ptr;
            ptr += sizeof(char);
        }
    }
}

std::string Transaction::toString() {
    std::string result;
    result += std::to_string(m_id);
    result += std::to_string(m_in.size());
    for(size_t i = 0; i < m_in.size(); i++) {
        result += std::to_string(m_in[i].m_tranId);
        result += std::to_string(m_in[i].m_outIndex);
        result += m_in[i].m_pubkey;
        result += m_in[i].m_sign;
    }
    result += std::to_string(m_out.size());
    for(size_t i = 0; i < m_out.size(); i++) {
        result += std::to_string(m_out[i].m_value);
        result += m_out[i].m_address;
    }

    return result;
}

void Transaction::print() const noexcept {
    std::cout << "|transaction id: " << m_id << "\n";
    std::cout << "|" << std::setfill('_') << std::setw(39) << "TXINPUTS" << std::setfill('_')
              << std::setw(40) << "\n";

    for(size_t i = 0; i < m_in.size(); i++) {
        std::cout << "|output id : " << m_in[i].m_tranId << "\n";
        std::cout << "|index: " << m_in[i].m_outIndex << "\n";
        std::cout << "|pubkey from: " << m_in[i].m_pubkey << "\n";
        std::cout << "|sign: " << m_in[i].m_sign << "\n";
        std::cout << "|" << std::setfill('-') << std::setw(79) << "\n";
    }
    std::cout << "|" << std::setfill('_') << std::setw(39) << "TXOUTPUTS" << std::setfill('_')
              << std::setw(40) << "\n";
    for(size_t i = 0; i < m_out.size(); i++) {
        std::cout << "|value: " << m_out[i].m_value << "\n";
        std::cout << "|address to: " << m_out[i].m_address << "\n";
        std::cout << "|" << std::setfill('-') << std::setw(79) << "\n";
    }
}

CoinBaseTransaction::CoinBaseTransaction(uint64_t& id, std::string& pubkey):
    Transaction(id, 0, 1),
    m_address(pubkey) {
    m_out.push_back(TXOutput(REWARD, pubkey));
}

RealTransaction::RealTransaction(
    const uint64_t& id,
    const std::string& from,
    const std::string& to,
    int value,
    std::list<TXInput>& inputs,
    int rest):
    Transaction(id, (int) inputs.size(), 0),
    m_from(from),
    m_to(to),
    m_value(value),
    m_rest(rest) {

    int i = 0;
    std::list<TXInput>::iterator it;
    for(it = inputs.begin(); it != inputs.end(); it++) {
        m_in[i] = *it;
        i++;
    }

    m_out.push_back(TXOutput(value, to));

    if(rest > 0) {
        m_out.push_back(TXOutput(rest, from));
    }
}

TXOutput::TXOutput(int value, const std::string& address) {
    m_value = value;
    m_address = address;
}

TXOutput::TXOutput(const TXOutput& out) {
    m_value = out.m_value;
    m_address = out.m_address;
}

TXOutput::TXOutput(const TXOutput&& out) {
    m_address = std::move(out.m_address);
    m_value = std::move(out.m_value);
}

TXOutput& TXOutput::operator=(const TXOutput& out) {
    if(this == &out) {
        return *this;
    }
    m_value = out.m_value;
    m_address = out.m_address;
    return *this;
}

TXOutput& TXOutput::operator=(const TXOutput&& out) {
    if(this == &out) {
        return *this;
    }

    m_value = std::move(out.m_value);
    m_address = std::move(out.m_address);

    return *this;
}

bool TXOutput::operator==(const TXOutput &out) const {
    bool flag = true;
    flag &= (this->m_address == out.m_address);
    flag &= (this->m_value == out.m_value);
    return flag;
}

TXInput::TXInput(const uint64_t& transId, int outIndex, const std::string& pubkey) {
    m_tranId = transId;
    m_outIndex = outIndex;
    m_pubkey = pubkey;
}

TXInput::TXInput(const TXInput& in) {
    m_tranId = in.m_tranId;
    m_outIndex = in.m_outIndex;
    m_pubkey = in.m_pubkey;
    m_sign = in.m_sign;
}

TXInput::TXInput(const TXInput&& in) {
    m_tranId = std::move(in.m_tranId);
    m_outIndex = std::move(in.m_outIndex);
    m_pubkey = std::move(in.m_pubkey);
    m_sign = std::move(in.m_sign);
}

TXInput& TXInput::operator=(const TXInput& in) {
    if(this == &in) {
        return *this;
    }
    m_tranId = in.m_tranId;
    m_outIndex = in.m_outIndex;
    m_pubkey = in.m_pubkey;
    m_sign = in.m_sign;
    return *this;
}

TXInput& TXInput::operator=(const TXInput&& in) {
    if(this == &in) {
        return *this;
    }

    m_tranId = std::move(in.m_tranId);
    m_outIndex = std::move(in.m_outIndex);
    m_pubkey = std::move(in.m_pubkey);
    m_sign = std::move(in.m_sign);

    return *this;
}

bool TXInput::operator == (const TXInput &in) const {
    bool flag = true;
    flag &= (this->m_outIndex == in.m_outIndex);
    flag &= (this->m_sign == in.m_sign);
    flag &= (this->m_pubkey == in.m_pubkey);
    flag &= (this->m_pubkey == in.m_pubkey);
    return flag;
}

bool Transaction::operator==(const Transaction &tx) const {
    bool flag = true;
    
    flag &= (this->m_id == tx.m_id);
    
    auto tx1_in_itr = this->m_in.begin();
    auto tx2_in_itr = tx.m_in.begin();
    
    while (tx1_in_itr != this->m_in.end() && tx2_in_itr != tx.m_in.end()){
        flag &= (*tx1_in_itr == *tx2_in_itr);
        ++tx1_in_itr;
        ++tx2_in_itr;
    }
    
    if (tx1_in_itr != this->m_in.end() || tx2_in_itr != tx.m_in.end()){
        return false;
    }
    
    auto tx1_out_itr = this->m_out.begin();
    auto tx2_out_itr = tx.m_out.begin();
    
    while (tx1_out_itr != this->m_out.end() && tx2_out_itr != tx.m_out.end()){
        flag &= (*tx1_out_itr == *tx2_out_itr);
        
        ++tx1_out_itr;
        ++tx2_out_itr;
    }
    
    if (tx1_out_itr != this->m_out.end() || tx2_out_itr != tx.m_out.end()){
        return false;
    }
    
    return flag;
}