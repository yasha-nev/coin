#include "crypto/CryptoppRsaProcessor.hpp"

CryptoppRsaProcessor::CryptoppRsaProcessor(const std::string& path, size_t bits):
    m_keysPath(path) {
    if(!loadKeys()) {
        generateAndSaveKeys(bits);
    }
}

std::string CryptoppRsaProcessor::getPublicKeyBase58() const {
    return keyToBase58(m_publicKey);
}

std::string CryptoppRsaProcessor::getPrivateKeyBase58() const {
    return keyToBase58(m_privateKey);
}

bool CryptoppRsaProcessor::loadKeys() {
    try {
        CryptoPP::FileSource pr((m_keysPath + ".priv").c_str(), true);
        m_privateKey.Load(pr);

        CryptoPP::FileSource pu((m_keysPath + ".pub").c_str(), true);
        m_publicKey.Load(pu);
        return true;
    } catch(...) {
        return false;
    }
}

void CryptoppRsaProcessor::generateAndSaveKeys(size_t bits) {
    m_privateKey.Initialize(m_rng, bits);
    m_publicKey.AssignFrom(m_privateKey);

    m_privateKey.Save(CryptoPP::FileSink((m_keysPath + ".priv").c_str()).Ref());
    m_publicKey.Save(CryptoPP::FileSink((m_keysPath + ".pub").c_str()).Ref());
}

std::unique_ptr<IRsaProcessor> IRsaProcessor::create(const std::string& keysPath, size_t keyBits) {
    return std::make_unique<CryptoppRsaProcessor>(keysPath, keyBits);
}
