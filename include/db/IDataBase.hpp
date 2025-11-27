#ifndef IDATABASE_hpp
#define IDATABASE_hpp

#include "Block.hpp"

#include <memory>

#define DBPATH "./testdb"

class IDataBase {
public:
    virtual ~IDataBase() = default;

    virtual void connect() = 0;

    virtual void putBlock(const std::unique_ptr<Block>& block) = 0;

    virtual std::array<uint32_t, 8> getCurrentHash() = 0;

    virtual uint64_t getCurrentId(const std::array<uint32_t, 8>& hash) = 0;

    virtual std::unique_ptr<Block> getBlockByHash(const std::array<uint32_t, 8>& hash) = 0;
};

#endif
