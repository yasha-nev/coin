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

    s += sizeof(size_t);
    for (const auto &in : m_in) {
        s += in.size();
    }

    s += sizeof(size_t);
    for (const auto &out : m_out) {
        s += out.size();
    }

    return s;
}

std::vector<std::byte> Transaction::encode() const{
    ByteWriter byteWriter;

    byteWriter.write<uint64_t>(m_id);

    byteWriter.write<size_t>(m_in.size());
    for (const auto &in : m_in) {
        auto encodeIn = in.encode();
        byteWriter.write_bytes(encodeIn.data(), encodeIn.size());
    }

    byteWriter.write<size_t>(m_out.size());
    for (const auto &out : m_out) {
        auto encodeOut = out.encode();
        byteWriter.write_bytes(encodeOut.data(), encodeOut.size());
    }
}

void Transaction::decode(const std::vector<std::byte> &data) {
    /*m_in.clear();
    m_out.clear();

    ByteReader byteReader(data);

    m_id = byteReader.read<uint64_t>();

    size_t inCount = byteReader.read<size_t>();

    m_in.resize(inCount);

    for(size_t i = 0; i < inCount; i++) {
        auto decodeIn = byteReader.read_bytes()
        m_in[i].decode()
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
    }*/
}

std::string Transaction::toString() {
    std::string result;
    result += std::to_string(m_id);
    result += std::to_string(m_in.size());
    for (const auto &in : m_in) {
        result += std::to_string(in.getTransactionId());
        result += std::to_string(in.getOutIndex());
        result += in.getPublicKey();
        result += in.getSignature();
    }
    result += std::to_string(m_out.size());
    for (const auto &out : m_out) {
        result += std::to_string(out.getValue());
        result += out.getAddress();
    }

    return result;
}

void Transaction::print() const noexcept {
    std::cout << "|transaction id: " << m_id << "\n";
    std::cout << "|" << std::setfill('_') << std::setw(39) << "TXINPUTS" << std::setfill('_')
              << std::setw(40) << "\n";

    for (const auto &in : m_in) {
        std::cout << "|output id : " << in.getTransactionId() << "\n";
        std::cout << "|index: " << in.getOutIndex() << "\n";
        std::cout << "|pubkey from: " << in.getPublicKey() << "\n";
        std::cout << "|sign: " << in.getSignature() << "\n";
        std::cout << "|" << std::setfill('-') << std::setw(79) << "\n";
    }
    std::cout << "|" << std::setfill('_') << std::setw(39) << "TXOUTPUTS" << std::setfill('_')
              << std::setw(40) << "\n";
    for (const auto &out : m_out) {
        std::cout << "|value: " << out.getValue() << "\n";
        std::cout << "|address to: " << out.getAddress() << "\n";
        std::cout << "|" << std::setfill('-') << std::setw(79) << "\n";
    }
}

uint64_t Transaction::getId() const noexcept {
    return m_id;
}

const std::vector<TXInput> &Transaction::getInputs() const noexcept {
    return m_in;
}

const std::vector<TXOutput> &Transaction::getOutputs() const noexcept {
    return m_out;
}

void Transaction::sign() {
    std::string signstr;

    for (const auto &out : m_out) {
        signstr += out.getAddress();
    }

    for (auto &in : m_in) {
        CryptoppImpl cryptor;
        std::string key = in.getPublicKey() + signstr;
        auto hash = cryptor.sha256Hash(key);
        in.setSignarure(cryptor.sha256HashToString(hash));
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