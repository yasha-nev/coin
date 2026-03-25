#pragma once

#include "Hash.hpp"

#include <memory>
#include <string>

class ICrypto {
public:
    virtual ~ICrypto() = default;

    virtual std::string encodeBase58(const std::string& source) = 0;

    virtual std::string decodeBase58(const std::string& str) = 0;

    virtual Hash sha256Hash(const std::string& message) = 0;

    virtual Hash ripemd160Hash(const std::string& message) = 0;

    virtual std::string sha256HashToString(const Hash& hash) = 0;

    virtual std::string ripemd160HashToString(const Hash& hash) = 0;
};

namespace crypto {
    std::unique_ptr<ICrypto> createCryptoppImpl();
}
