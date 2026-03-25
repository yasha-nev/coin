#pragma once

#include "Hash.hpp"
#include "IHashEncoder.hpp"
#include "base58.hpp"

#include <cryptopp/base64.h>
#include <cryptopp/cryptlib.h>
#include <cryptopp/filters.h>
#include <cryptopp/osrng.h>
#include <cryptopp/ripemd.h>
#include <cryptopp/sha.h>

#include <cstdint>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <vector>

class CryptoppHashEncoder: public IHashEncoder {
public:
    std::string encodeBase58(const std::string& source) override;

    std::string decodeBase58(const std::string& str) override;

    Hash sha256Hash(const std::string& message) override;

    Hash ripemd160Hash(const std::string& message) override;

    std::string sha256HashToString(const Hash& hash) override;

    std::string ripemd160HashToString(const Hash& hash) override;
};
