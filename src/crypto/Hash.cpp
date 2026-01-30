#include "crypto/Hash.hpp"

#define SHA256_HASH_SIZE 32

const std::vector<uint8_t> Hash::getHashAsVector() const noexcept {
    return m_hash;
}

void Hash::encode(ByteWriter& writer) const {
    writer.write<size_t>(m_hash.size());
    writer.write_bytes(as_bytes(m_hash));
}

void Hash::decode(ByteReader& reader) {
    auto hashSize = reader.read<size_t>();
    m_hash.clear();

    std::span<const std::byte> bytes = reader.read_bytes(hashSize);

    for(size_t i = 0; i < bytes.size(); i++) {
        m_hash.push_back(std::to_integer<uint8_t>(bytes[i]));
    }
}

Hash& Hash::operator=(const Hash& other) {
    if(this != &other) {
        m_hash = other.m_hash;
    }
    return *this;
}

Hash& Hash::operator=(Hash&& other) {
    if(this != &other) {
        m_hash = std::move(other.m_hash);
    }
    return *this;
}

bool Hash::operator<(const Hash& other) const {
    return std::lexicographical_compare(
        m_hash.begin(), m_hash.end(), other.m_hash.begin(), other.m_hash.end());
}

bool Hash::operator>(const Hash& other) const {
    return other < *this;
}

bool operator==(const Hash& lhs, const Hash& rhs) {
    return lhs.m_hash == rhs.m_hash;
}

Hash createZeroHash() {
    return Hash({ std::vector<uint8_t>(SHA256_HASH_SIZE, 0) });
}

bool checkZeroHash(const Hash& hash) {
    for(auto byte: hash.m_hash) {
        if(byte != 0) {
            return false;
        }
    }
    return true;
}
