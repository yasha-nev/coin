#include "TXInput.hpp"

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

uint64_t TXInput::getTransactionId() const noexcept {
    return m_tranId;
}

int TXInput::getOutIndex() const noexcept {
    return m_outIndex;
}

const std::string& TXInput::getPublicKey() const noexcept {
    return m_pubkey;
}

const std::string& TXInput::getSignature() const noexcept {
    return m_sign;
}

void TXInput::setSignarure(const std::string& signature) {
    m_sign = signature;
}

void TXInput::encode(ByteWriter& byteWriter) const {

    byteWriter.write<uint64_t>(m_tranId);
    byteWriter.write<int>(m_outIndex);

    byteWriter.write<size_t>(m_pubkey.size());
    for(size_t i = 0; i < m_pubkey.size(); i++) {
        byteWriter.write<char>(m_pubkey[i]);
    }

    byteWriter.write<size_t>(m_sign.size());
    for(size_t i = 0; i < m_sign.size(); i++) {
        byteWriter.write<char>(m_sign[i]);
    }
}

void TXInput::decode(ByteReader& byteReader) {

    m_tranId = byteReader.read<uint64_t>();
    m_outIndex = byteReader.read<int>();

    size_t pubkeySize = byteReader.read<size_t>();
    m_pubkey.resize(pubkeySize);
    for(size_t i = 0; i < m_pubkey.size(); i++) {
        m_pubkey[i] = byteReader.read<char>();
    }

    size_t signSize = byteReader.read<size_t>();
    m_sign.resize(signSize);
    for(size_t i = 0; i < m_sign.size(); i++) {
        m_sign[i] = byteReader.read<char>();
    }
}

size_t TXInput::size() const noexcept {
    size_t s = 0;
    s += sizeof(uint64_t);
    s += sizeof(int);
    s += sizeof(size_t);
    s += sizeof(char) * m_pubkey.size();
    s += sizeof(size_t);
    s += sizeof(char) * m_sign.size();
    return s;
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

bool TXInput::operator==(const TXInput& in) const {
    bool flag = true;
    flag &= (this->m_outIndex == in.m_outIndex);
    flag &= (this->m_sign == in.m_sign);
    flag &= (this->m_pubkey == in.m_pubkey);
    flag &= (this->m_pubkey == in.m_pubkey);
    return flag;
}
