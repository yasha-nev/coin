#include "net/Message.hpp"

Message::~Message() {
}

MsgType Message::getCommand() noexcept {
    return m_header.command;
}

void Message::encodeHeader(ByteWriter& byteWriter) const {
    byteWriter.write_bytes(as_bytes(m_header.startString.data(), START_STRING_SIZE));
    byteWriter.write<uint8_t>(static_cast<uint8_t>(m_header.command));
    byteWriter.write<size_t>(m_header.payloadSize);
    byteWriter.write_bytes(as_bytes(m_header.checkSum.data(), CHECK_SUM_SIZE));
}

void Message::decodeHeader(ByteReader& byteReader) {
    auto startString = byteReader.read_bytes(START_STRING_SIZE);
    std::memcpy(m_header.startString.data(), startString.data(), START_STRING_SIZE);
    m_header.command = static_cast<MsgType>(byteReader.read<uint8_t>());
    m_header.payloadSize = byteReader.read<size_t>();
    auto checksum = byteReader.read_bytes(CHECK_SUM_SIZE);
    std::memcpy(m_header.checkSum.data(), checksum.data(), CHECK_SUM_SIZE);
}

GetBlocksMsg::GetBlocksMsg():
    m_version(0) {
    memcpy(m_header.startString.data(), START_STRING, START_STRING_SIZE);
    m_header.command = MsgType::gBlocks;
    // Protocol version + hashe Count + hashes
    m_header.payloadSize = sizeof(uint8_t) + sizeof(size_t);
}

GetBlocksMsg::GetBlocksMsg(const std::list<Hash>& hashes):
    m_version(0),
    m_hashes(hashes) {
    memcpy(m_header.startString.data(), START_STRING, START_STRING_SIZE);
    m_header.command = MsgType::gBlocks;
    // Protocol version + hashe Count + hashes + zero_hash
    m_header.payloadSize = sizeof(uint8_t) + sizeof(size_t) +
        sizeof(uint32_t) * 8 * (hashes.size() + 1);
}

const std::list<Hash>& GetBlocksMsg::getHashes() noexcept {
    return m_hashes;
}

void GetBlocksMsg::decode(ByteReader& byteReader) {
    decodeHeader(byteReader);

    m_version = byteReader.read<uint8_t>();
    size_t count = byteReader.read<size_t>();
    for(size_t i = 0; i < count - 1; i++) {
        Hash hash;
        hash.decode(byteReader);
        m_hashes.push_back(hash);
    }
}

void GetBlocksMsg::encode(ByteWriter& byteWriter) const {
    encodeHeader(byteWriter);

    byteWriter.write<uint8_t>(m_version);

    size_t count = m_hashes.size() + 1;
    byteWriter.write<size_t>(count);

    for(const auto& hash: m_hashes) {
        hash.encode(byteWriter);
    }

    uint8_t zerohash[32] = { 0 };
    byteWriter.write_bytes(as_bytes(zerohash, sizeof(uint8_t) * 32));
}

void GetBlocksMsg::print() const noexcept {
    auto cryptor = crypto::createCryptoppImpl();
    std::cout << "GetBlocksMsg\n";
    for(auto& itr: m_hashes) {
        std::cout << cryptor->sha256HashToString(itr) << "\n";
    }
    std::cout << std::endl;
}

InvMsg::InvMsg():
    m_type(InvTypes::iBlock) {
    memcpy(m_header.startString.data(), START_STRING, START_STRING_SIZE);
    m_header.command = MsgType::Inv;
    // Hashe Count + unique identif + hashes + zero_hash
    m_header.payloadSize = sizeof(size_t) + sizeof(uint8_t);
}

InvMsg::InvMsg(InvTypes type, const std::list<Hash>& hashes):
    m_type(type),
    m_hashes(hashes) {
    memcpy(m_header.startString.data(), START_STRING, START_STRING_SIZE);
    m_header.command = MsgType::Inv;
    // Hashe Count + unique identif + hashes + zero_hash
    m_header.payloadSize = sizeof(size_t) + sizeof(uint8_t) +
        sizeof(uint32_t) * 8 * (m_hashes.size() + 1);
}

const std::list<Hash>& InvMsg::getHashes() noexcept {
    return m_hashes;
}

void InvMsg::decode(ByteReader& byteReader) {

    decodeHeader(byteReader);

    size_t count = byteReader.read<size_t>();
    for(size_t i = 0; i < count - 1; i++) {
        m_type = static_cast<InvTypes>(byteReader.read<uint8_t>());
        Hash hash;
        hash.decode(byteReader);
        m_hashes.push_back(hash);
    }
}

void InvMsg::encode(ByteWriter& byteWriter) const {
    encodeHeader(byteWriter);

    size_t count = m_hashes.size() + 1;
    byteWriter.write<size_t>(count);
    for(const auto& hash: m_hashes) {
        byteWriter.write<uint8_t>(static_cast<uint8_t>(m_type));
        hash.encode(byteWriter);
    }
    uint8_t zerohash[32] = { 0 };
    byteWriter.write_bytes(as_bytes(zerohash, sizeof(uint8_t) * 32));
}

void InvMsg::print() const noexcept {
    auto cryptor = crypto::createCryptoppImpl();
    std::cout << "InvMsg\n";
    std::cout << "Type: " << static_cast<uint8_t>(m_type) << "\n";
    for(auto& itr: m_hashes) {
        std::cout << cryptor->sha256HashToString(itr) << "\n";
    }
    std::cout << std::endl;
}

GetDataMsg::GetDataMsg():
    m_type(DataTypes::dBlock) {
    memcpy(m_header.startString.data(), START_STRING, START_STRING_SIZE);
    m_header.command = MsgType::gData;
    // number of  objects + unique identif + hashes + zero_hash
    m_header.payloadSize = sizeof(size_t) + sizeof(uint8_t);
}

GetDataMsg::GetDataMsg(DataTypes type, const std::list<Hash>& hashes):
    m_type(type),
    m_hashes(hashes) {
    memcpy(m_header.startString.data(), START_STRING, START_STRING_SIZE);
    m_header.command = MsgType::gData;
    // number of  objects + unique identif + hashes + zero_hash
    m_header.payloadSize = sizeof(size_t) + sizeof(uint8_t) +
        sizeof(uint32_t) * 8 * (hashes.size() + 1);
}

const std::list<Hash>& GetDataMsg::getHashes() noexcept {
    return m_hashes;
}

void GetDataMsg::decode(ByteReader& byteReader) {

    decodeHeader(byteReader);

    size_t count = byteReader.read<size_t>();
    for(size_t i = 0; i < count - 1; i++) {
        m_type = static_cast<DataTypes>(byteReader.read<uint8_t>());
        Hash hash;
        hash.decode(byteReader);
        m_hashes.push_back(hash);
    }
}

void GetDataMsg::encode(ByteWriter& byteWriter) const {

    encodeHeader(byteWriter);

    size_t count = m_hashes.size() + 1;
    byteWriter.write<size_t>(count);
    for(const auto& hash: m_hashes) {
        byteWriter.write<uint8_t>(static_cast<uint8_t>(m_type));
        hash.encode(byteWriter);
    }
    uint8_t zerohash[32] = { 0 };
    byteWriter.write_bytes(as_bytes(zerohash, sizeof(uint32_t) * 32));
}

void GetDataMsg::print() const noexcept {
    auto cryptor = crypto::createCryptoppImpl();
    std::cout << "GetDataMsg\n";
    std::cout << "Type: " << static_cast<int>(m_type) << "\n";
    for(auto& itr: m_hashes) {
        std::cout << cryptor->sha256HashToString(itr) << "\n";
    }
    std::cout << std::endl;
}

BlockMsg::BlockMsg() {
    memcpy(m_header.startString.data(), START_STRING, START_STRING_SIZE);
    m_header.command = MsgType::sBlock;
    // number of  objects
    m_header.payloadSize = 0;
}

BlockMsg::BlockMsg(const Block& block):
    m_block(block) {
    memcpy(m_header.startString.data(), START_STRING, START_STRING_SIZE);
    m_header.command = MsgType::sBlock;
    // block size
    m_header.payloadSize = m_block.size();
}

const Block& BlockMsg::getBlock() noexcept {
    return m_block;
}

void BlockMsg::decode(ByteReader& byteReader) {
    decodeHeader(byteReader);
    m_block.decode(byteReader);
}

void BlockMsg::encode(ByteWriter& byteWriter) const {
    encodeHeader(byteWriter);
    m_block.encode(byteWriter);
}

void BlockMsg::print() const noexcept {
    std::cout << "BlockMsg\n";
    m_block.print();
    std::cout << std::endl;
}

TxMsg::TxMsg() {
    memcpy(m_header.startString.data(), START_STRING, START_STRING_SIZE);
    m_header.command = MsgType::Tx;
    // Transaction size
    m_header.payloadSize = 0;
}

TxMsg::TxMsg(const Transaction& tx):
    m_tx(tx) {
    memcpy(m_header.startString.data(), START_STRING, START_STRING_SIZE);
    m_header.command = MsgType::Tx;
    // Transaction size
    m_header.payloadSize = m_tx.size();
}

const Transaction& TxMsg::getTransaction() noexcept {
    return m_tx;
}

void TxMsg::decode(ByteReader& byteReader) {
    decodeHeader(byteReader);
    m_tx.decode(byteReader);
}

void TxMsg::encode(ByteWriter& byteWriter) const {
    encodeHeader(byteWriter);
    m_tx.encode(byteWriter);
}

void TxMsg::print() const noexcept {
    std::cout << "TxMsg\n";
    m_tx.print();
    std::cout << std::endl;
}

NoFoundMsg::NoFoundMsg() {
    memcpy(m_header.startString.data(), START_STRING, START_STRING_SIZE);
    m_header.command = MsgType::noFound;
    m_header.payloadSize = 0;
}

void NoFoundMsg::decode(ByteReader& byteReader) {
    decodeHeader(byteReader);
}

void NoFoundMsg::encode(ByteWriter& byteWriter) const {
    encodeHeader(byteWriter);
}

void NoFoundMsg::print() const noexcept {
    std::cout << "NoFoundMsg\n";
    std::cout << std::endl;
}
