#pragma once

#include "IRsaProcessor.hpp"
#include "base58.hpp"

#include <cryptopp/files.h>
#include <cryptopp/osrng.h>
#include <cryptopp/rsa.h>

#include <fstream>

class CryptoppRsaProcessor: public IRsaProcessor {
public:
    CryptoppRsaProcessor(const std::string& path, size_t bits);

    std::string getPublicKeyBase58() const override;

    std::string getPrivateKeyBase58() const override;

private:
    bool loadKeys();

    void generateAndSaveKeys(size_t bits);

    template<typename T>
    std::string keyToBase58(const T& key) const {
        std::string binary;
        CryptoPP::StringSink ss(binary);
        key.Save(ss);
        return EncodeBase58(binary);
    }

    std::string m_keysPath;
    CryptoPP::AutoSeededRandomPool m_rng;
    CryptoPP::RSA::PrivateKey m_privateKey;
    CryptoPP::RSA::PublicKey m_publicKey;
};
