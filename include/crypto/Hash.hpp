
#ifndef Hash_hpp
#define Hash_hpp

#include "ByteReader.hpp"
#include "ByteWriter.hpp"
#include "Serializer.hpp"

#include <inttypes.h>
#include <vector>

class Hash: public Serializer {
public:
    Hash() {
    }

    Hash(const std::vector<uint8_t>& hash):
        m_hash(hash) {
    }

    Hash(const Hash& other):
        m_hash(other.m_hash) {
    }

    Hash(Hash&& other) noexcept:
        m_hash(std::move(other.m_hash)) {
    }

    ~Hash() = default;

    void encode(ByteWriter& writer) const override;

    void decode(ByteReader& reader) override;

    Hash& operator=(const Hash& hash);

    Hash& operator=(Hash&& hash);

    bool operator<(const Hash& other) const;

    bool operator>(const Hash& other) const;

    const std::vector<uint8_t> getHashAsVector() const noexcept;

    friend bool operator==(const Hash& lHash, const Hash& rHash);

    friend bool checkZeroHash(const Hash& hash);

    friend Hash createZeroHash();

private:
    std::vector<uint8_t> m_hash;
};

Hash createZeroHash();

bool checkZeroHash(const Hash& hash);

#endif
