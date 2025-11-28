#ifndef CRYPTOPPIMPL_hpp
#define CRYPTOPPIMPL_hpp

#include "ICrypto.hpp"
#include "base58.hpp"

#include <cryptopp/cryptlib.h>
#include <cryptopp/osrng.h>
#include <cryptopp/filters.h>
#include <cryptopp/sha.h>
#include <cryptopp/ripemd.h>
#include <cryptopp/base64.h>
#include <iostream>
#include <iomanip>
#include <vector>
#include <cstdint>
#include <array>
#include <sstream>

class CryptoppImpl : public ICrypto {
public:
    std::string encodeBase58(const std::string &source) override;

    std::string decodeBase58(const std::string &str) override;

    std::array<uint8_t, 32> sha256Hash(const std::string &message) override;

    std::array<uint8_t, 20> ripemd160Hash(const std::string &message) override;

    std::string sha256HashToString(const std::array<uint8_t, 32> &array) override;

    std::string ripemd160HashToString(const std::array<uint8_t, 20> &array) override;
};

#endif