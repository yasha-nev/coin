#include "crypto/CryptoppHashEncoder.hpp"

#define SHA256_HASH_SIZE 32
#define RIPMD160_HASH_SIZE 20

std::string CryptoppHashEncoder::encodeBase58(const std::string& source) {
    return EncodeBase58(source);
}

std::string CryptoppHashEncoder::decodeBase58(const std::string& str) {
    return DecodeBase58(str);
}

Hash CryptoppHashEncoder::sha256Hash(const std::string& message) {
    CryptoPP::SHA256 hash;

    std::vector<CryptoPP::byte> byteArray(message.begin(), message.end());

    std::vector<uint8_t> digest(SHA256_HASH_SIZE);
    hash.CalculateDigest(digest.data(), byteArray.data(), byteArray.size());
    return Hash(digest);
}

Hash CryptoppHashEncoder::ripemd160Hash(const std::string& message) {
    CryptoPP::RIPEMD160 hash;

    std::vector<CryptoPP::byte> byteArray(message.begin(), message.end());

    std::vector<uint8_t> digest(RIPMD160_HASH_SIZE);
    hash.CalculateDigest(digest.data(), byteArray.data(), byteArray.size());
    return Hash(digest);
}

template<typename ArrayType>
static std::string toHex(const ArrayType& arr) {
    std::stringstream ss;
    for(auto val: arr) {
        ss << std::setfill('0') << std::setw(2) << std::hex << val;
    }
    return ss.str();
}

std::string CryptoppHashEncoder::sha256HashToString(const Hash& hash) {
    std::stringstream s;

    const auto& array = hash.getHashAsVector();

    for(int i = 0; i < 8; i++) {
        uint32_t value = 0;
        for(int j = 0; j < 4; j++) {
            value |= (array[j + 4 * i]) << ((3 - j) * 8);
        }
        s << std::setfill('0') << std::setw(8) << std::hex << value;
    }
    return s.str();
}

std::string CryptoppHashEncoder::ripemd160HashToString(const Hash& hash) {
    const auto& array = hash.getHashAsVector();
    return toHex(array);
}

std::unique_ptr<IHashEncoder> IHashEncoder::create() {
    return std::make_unique<CryptoppHashEncoder>();
}
