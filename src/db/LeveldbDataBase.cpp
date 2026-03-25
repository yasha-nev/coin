#include "db/LeveldbDataBase.hpp"

void LeveldbDataBase::connect() {
    leveldb::Options options;
    options.create_if_missing = true;

    leveldb::DB* rawDb = nullptr;
    leveldb::Status status = leveldb::DB::Open(options, DBPATH, &rawDb);
    if(!status.ok()) {
        throw std::runtime_error("Failed to open/create LevelDB: " + status.ToString());
    }

    m_db = std::unique_ptr<leveldb::DB>(rawDb);
}

std::optional<Hash> LeveldbDataBase::getCurrentHash() {
    Hash res;
    std::string hash;

    leveldb::Status status = m_db->Get(leveldb::ReadOptions(), "l", &hash);

    if(status.IsIOError() || status.IsNotFound()) {
        return std::nullopt;
    }

    ByteReader byteReader(as_bytes(hash.data(), hash.size()));
    res.decode(byteReader);

    return res;
}

std::optional<uint64_t> LeveldbDataBase::getCurrentId(const Hash& hash) {
    auto block = getBlockByHash(hash);

    if(!block.has_value()) {
        return std::nullopt;
    }

    const auto& transactions = block->getTransactions();

    if(transactions.empty()) {
        return std::nullopt;
    }

    const auto& lastTransaction = transactions.back();
    uint64_t id = lastTransaction.getId();
    return id;
}

std::optional<Block> LeveldbDataBase::getBlockByHash(const Hash& hash) {
    std::string byteBlock;
    ByteWriter byteWriter;

    hash.encode(byteWriter);
    const auto& bytes = byteWriter.bytes();

    leveldb::Slice key((char*) bytes.data(), bytes.size());
    leveldb::Status status = m_db->Get(leveldb::ReadOptions(), key, &byteBlock);

    if(status.IsIOError() || status.IsNotFound()) {
        return std::nullopt;
    }

    Block block = Block();

    ByteReader byteReader(as_bytes(byteBlock.data(), byteBlock.size()));
    block.decode(byteReader);

    return block;
}

void LeveldbDataBase::putBlock(const Block& block) {
    ByteWriter hashByteWriter;
    const Hash& hash = block.getHash();
    hash.encode(hashByteWriter);
    const auto& hashBytes = hashByteWriter.bytes();

    leveldb::Slice key((char*) hashBytes.data(), hashBytes.size());

    ByteWriter blockByteWriter;
    block.encode(blockByteWriter);
    const auto& blockBytes = blockByteWriter.bytes();

    leveldb::Slice value((char*) blockBytes.data(), blockBytes.size());
    leveldb::Slice value_hash((char*) hashBytes.data(), hashBytes.size());

    m_db->Put(leveldb::WriteOptions(), key, value);
    m_db->Put(leveldb::WriteOptions(), "l", value_hash);
}
