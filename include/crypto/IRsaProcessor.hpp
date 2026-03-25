#pragma once

#include <memory>
#include <string>
#include <vector>

class IRsaProcessor {
public:
    virtual ~IRsaProcessor() = default;

    virtual std::string getPublicKeyBase58() const = 0;

    virtual std::string getPrivateKeyBase58() const = 0;

    static std::unique_ptr<IRsaProcessor> create(
        const std::string& keysPath,
        size_t keyBits = 2048);
};
