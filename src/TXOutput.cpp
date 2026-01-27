#include "TXOutput.hpp"

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

std::vector<std::byte> TXOutput::encode() const {
    ByteWriter byteWriter;

    byteWriter.write<int>(m_value);
    byteWriter.write<size_t>(m_address.size());
    for(size_t i = 0; i < m_address.size(); i++) {
        byteWriter.write<char>(m_address[i]);
    }

    return byteWriter.bytes();
}

void TXOutput::decode(const std::vector<std::byte>& data) {
    ByteReader byteReader(data);

    m_value = byteReader.read<int>();
    size_t addressSize = byteReader.read<size_t>();
    m_address.resize(addressSize);
    for(size_t i = 0; i < addressSize; i++) {
        m_address[i] = byteReader.read<char>();
    }
}

int TXOutput::getValue() const noexcept {
    return m_value;
}

const std::string& TXOutput::getAddress() const noexcept {
    return m_address;
}

size_t TXOutput::size() const noexcept {
    size_t s = 0;

    s += sizeof(int);
    s += sizeof(size_t);
    s += sizeof(char) * m_address.size();

    return s;
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

bool TXOutput::operator==(const TXOutput& out) const {
    bool flag = true;
    flag &= (this->m_address == out.m_address);
    flag &= (this->m_value == out.m_value);
    return flag;
}
