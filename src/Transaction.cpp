#include "Transaction.hpp"

Transaction::Transaction() {
    m_id = 0;
}

Transaction::Transaction(uint64_t id, int inCount, int outCount) {
    m_id = id;
    m_in = std::vector<TXInput>(inCount);
    m_out = std::vector<TXOutput>(outCount);
}

Transaction::~Transaction() {
}

uint64_t Transaction::getId() const noexcept {
    return m_id;
}

const std::vector<TXInput>& Transaction::getInputs() const noexcept {
    return m_in;
}

const std::vector<TXOutput>& Transaction::getOutputs() const noexcept {
    return m_out;
}

size_t Transaction::size() const noexcept {
    size_t s = 0;

    s += sizeof(uint64_t);

    s += sizeof(size_t);
    for(const auto& in: m_in) {
        s += in.size();
    }

    s += sizeof(size_t);
    for(const auto& out: m_out) {
        s += out.size();
    }

    return s;
}

void Transaction::encode(ByteWriter& byteWriter) const {

    byteWriter.write<uint64_t>(m_id);

    byteWriter.write<size_t>(m_in.size());
    for(const auto& in: m_in) {
        in.encode(byteWriter);
    }

    byteWriter.write<size_t>(m_out.size());
    for(const auto& out: m_out) {
        out.encode(byteWriter);
    }
}

void Transaction::decode(ByteReader& byteReader) {

    m_id = byteReader.read<uint64_t>();

    size_t inCount = byteReader.read<size_t>();
    for(int i = 0; i < inCount; i++) {
        TXInput input;
        input.decode(byteReader);
        m_in.emplace_back(input);
    }
    size_t outCount = byteReader.read<size_t>();
    for(int i = 0; i < outCount; i++) {
        TXOutput output;
        output.decode(byteReader);
        m_out.push_back(output);
    }
}

std::string Transaction::toString() {
    std::string result;
    result += std::to_string(m_id);
    result += std::to_string(m_in.size());
    for(const auto& in: m_in) {
        result += std::to_string(in.getTransactionId());
        result += std::to_string(in.getOutIndex());
        result += in.getPublicKey();
        result += in.getSignature();
    }
    result += std::to_string(m_out.size());
    for(const auto& out: m_out) {
        result += std::to_string(out.getValue());
        result += out.getAddress();
    }

    return result;
}

void Transaction::print() const noexcept {
    std::cout << "|transaction id: " << m_id << "\n";
    std::cout << "|" << std::setfill('_') << std::setw(39) << "TXINPUTS" << std::setfill('_')
              << std::setw(40) << "\n";

    for(const auto& in: m_in) {
        std::cout << "|output id : " << in.getTransactionId() << "\n";
        std::cout << "|index: " << in.getOutIndex() << "\n";
        std::cout << "|pubkey from: " << in.getPublicKey() << "\n";
        std::cout << "|sign: " << in.getSignature() << "\n";
        std::cout << "|" << std::setfill('-') << std::setw(79) << "\n";
    }
    std::cout << "|" << std::setfill('_') << std::setw(39) << "TXOUTPUTS" << std::setfill('_')
              << std::setw(40) << "\n";
    for(const auto& out: m_out) {
        std::cout << "|value: " << out.getValue() << "\n";
        std::cout << "|address to: " << out.getAddress() << "\n";
        std::cout << "|" << std::setfill('-') << std::setw(79) << "\n";
    }
}

void Transaction::sign() {
    std::string signstr;

    for(const auto& out: m_out) {
        signstr += out.getAddress();
    }

    for(auto& in: m_in) {
        CryptoppImpl cryptor;
        std::string key = in.getPublicKey() + signstr;
        auto hash = cryptor.sha256Hash(key);
        in.setSignarure(cryptor.sha256HashToString(hash));
    }
}

bool Transaction::operator==(const Transaction& tx) const {
    bool flag = true;

    flag &= (this->m_id == tx.m_id);

    auto tx1_in_itr = this->m_in.begin();
    auto tx2_in_itr = tx.m_in.begin();

    while(tx1_in_itr != this->m_in.end() && tx2_in_itr != tx.m_in.end()) {
        flag &= (*tx1_in_itr == *tx2_in_itr);
        ++tx1_in_itr;
        ++tx2_in_itr;
    }

    if(tx1_in_itr != this->m_in.end() || tx2_in_itr != tx.m_in.end()) {
        return false;
    }

    auto tx1_out_itr = this->m_out.begin();
    auto tx2_out_itr = tx.m_out.begin();

    while(tx1_out_itr != this->m_out.end() && tx2_out_itr != tx.m_out.end()) {
        flag &= (*tx1_out_itr == *tx2_out_itr);

        ++tx1_out_itr;
        ++tx2_out_itr;
    }

    if(tx1_out_itr != this->m_out.end() || tx2_out_itr != tx.m_out.end()) {
        return false;
    }

    return flag;
}

Transaction TransactionFactory::createCoinBase(const uint64_t& id, const std::string& pubkey) {
    Transaction transaction(id, 0, 1);
    transaction.m_out.push_back(TXOutput(REWARD, pubkey));

    transaction.sign();

    return transaction;
}

Transaction TransactionFactory::createSimple(
    const uint64_t& id,
    const std::string& from,
    const std::string& to,
    int value,
    std::list<TXInput>& inputs,
    int rest) {

    Transaction transaction(id, (int) inputs.size(), 0);

    int i = 0;
    std::list<TXInput>::iterator it;
    for(it = inputs.begin(); it != inputs.end(); it++) {
        transaction.m_in[i] = *it;
        i++;
    }

    transaction.m_out.push_back(TXOutput(value, to));

    if(rest > 0) {
        transaction.m_out.push_back(TXOutput(rest, from));
    }

    transaction.sign();

    return transaction;
}
