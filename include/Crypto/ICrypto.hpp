#ifndef ICRYPTO_hpp
#define ICRYPTO_hpp

#include <string>
#include <array>

class ICrypto {
public:
    virtual ~ICrypto() = default;

    virtual std::string encodeBase58(const std::string &source) = 0;

    virtual std::string decodeBase58(const std::string &str) = 0;

    virtual std::array<uint8_t, 32> sha256Hash(const std::string &message) = 0;

    virtual std::array<uint8_t, 20> ripemd160Hash(const std::string &message) = 0;

    virtual std::string sha256HashToString(const std::array<uint8_t, 32> &array) = 0;

    virtual std::string ripemd160HashToString(const std::array<uint8_t, 20> &array) = 0;
};

#endif