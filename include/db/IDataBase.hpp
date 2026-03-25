#pragma once

#include "Block.hpp"

#include <memory>
#include <optional>

#define DBPATH "./testdb"

class IDataBase {
public:
    virtual ~IDataBase() = default;

    virtual void connect() = 0;

    virtual void putBlock(const Block& block) = 0;

    virtual std::optional<Hash> getCurrentHash() = 0;

    virtual std::optional<uint64_t> getCurrentId(const Hash& hash) = 0;

    virtual std::optional<Block> getBlockByHash(const Hash& hash) = 0;
};
