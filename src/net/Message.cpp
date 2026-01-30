#include "Message.hpp"

#define STARTSIZE 9

Message::~Message() {
}

MsgTypes Message::getCommand() noexcept {
    return m_comm;
}

void Message::setClientId(ClientID id) {
    m_clientId = id;
}

ClientID Message::getClientId() noexcept {
    return m_clientId;
}

void Message::encodeHeader(ByteWriter& byteWriter, size_t payloadSize, int checkSum) const {
    std::string startString = STARTSTR;
    byteWriter.write_bytes(as_bytes(startString.data(), startString.size()));
    byteWriter.write<uint8_t>(static_cast<uint8_t>(m_comm));
    byteWriter.write<size_t>(payloadSize);
    byteWriter.write<int>(checkSum);
}

void Message::decodeHeader(ByteReader& byteReader) {
    std::string startString = STARTSTR;
    byteReader.read_bytes(startString.size());
    m_comm = static_cast<MsgTypes>(byteReader.read<uint8_t>());
    byteReader.read<size_t>();
    byteReader.read<int>();
}

GetBlocksMsg::GetBlocksMsg() {
    m_comm = MsgTypes::gBlocks;
    m_ver = 0;
}

GetBlocksMsg::GetBlocksMsg(const std::list<Hash>& hashes) {
    m_comm = MsgTypes::gBlocks;
    m_ver = 0;
    m_hashes = hashes;
}

const std::list<Hash>& GetBlocksMsg::getHashes() noexcept {
    return m_hashes;
}

void GetBlocksMsg::decode(ByteReader& byteReader) {
    decodeHeader(byteReader);

    m_ver = byteReader.read<uint8_t>();
    size_t count = byteReader.read<size_t>();
    for(size_t i = 0; i < count - 1; i++) {
        Hash hash;
        hash.decode(byteReader);
        m_hashes.push_back(hash);
    }
}

void GetBlocksMsg::encode(ByteWriter& byteWriter) const {
    size_t count = m_hashes.size() + 1;
    size_t payloadSize = sizeof(uint8_t) + sizeof(size_t) + sizeof(uint32_t) * 8 * (count);

    encodeHeader(byteWriter, payloadSize, 0);

    byteWriter.write<uint8_t>(m_ver);
    byteWriter.write<size_t>(count);
    for(const auto& hash: m_hashes) {
        hash.encode(byteWriter);
    }

    uint8_t zerohash[32] = { 0 };
    byteWriter.write_bytes(as_bytes(zerohash, sizeof(uint8_t) * 32));
}

void GetBlocksMsg::print() const noexcept {
    CryptoppImpl cryptor;
    std::cout << "GetBlocksMsg\n";
    for(auto& itr: m_hashes) {
        std::cout << cryptor.sha256HashToString(itr) << "\n";
    }
    std::cout << std::endl;
}

InvMsg::InvMsg() {
    m_comm = MsgTypes::Inv;
    m_type = InvTypes::iBlock;
}

InvMsg::InvMsg(InvTypes type, const std::list<Hash>& hashes) {
    m_comm = MsgTypes::Inv;
    m_hashes = hashes;
    m_type = type;
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
    size_t count = m_hashes.size() + 1;
    size_t payloadSize = sizeof(size_t) + sizeof(uint8_t) * (m_hashes.size()) +
        sizeof(uint8_t) * 32 * (count);

    encodeHeader(byteWriter, payloadSize, 0);

    byteWriter.write<size_t>(count);
    for(const auto& hash: m_hashes) {
        byteWriter.write<uint8_t>(static_cast<uint8_t>(m_type));
        hash.encode(byteWriter);
    }
    uint8_t zerohash[32] = { 0 };
    byteWriter.write_bytes(as_bytes(zerohash, sizeof(uint8_t) * 32));
}

void InvMsg::print() const noexcept {
    CryptoppImpl cryptor;
    std::cout << "InvMsg\n";
    std::cout << "Type: " << static_cast<uint8_t>(m_type) << "\n";
    for(auto& itr: m_hashes) {
        std::cout << cryptor.sha256HashToString(itr) << "\n";
    }
    std::cout << std::endl;
}

GetDataMsg::GetDataMsg() {
    m_comm = MsgTypes::gData;
    m_type = DataTypes::dBlock;
}

GetDataMsg::GetDataMsg(DataTypes type, const std::list<Hash>& hashes) {
    m_comm = MsgTypes::gData;
    m_hashes = hashes;
    m_type = type;
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
    size_t count = m_hashes.size() + 1;
    size_t payloadSize = sizeof(size_t) + sizeof(uint8_t) * (m_hashes.size()) +
        sizeof(uint32_t) * 8 * (count);

    encodeHeader(byteWriter, payloadSize, 0);

    byteWriter.write<size_t>(count);
    for(const auto& hash: m_hashes) {
        byteWriter.write<uint8_t>(static_cast<uint8_t>(m_type));
        hash.encode(byteWriter);
    }
    uint8_t zerohash[32] = { 0 };
    byteWriter.write_bytes(as_bytes(zerohash, sizeof(uint32_t) * 32));
}

void GetDataMsg::print() const noexcept {
    CryptoppImpl cryptor;
    std::cout << "GetDataMsg\n";
    std::cout << "Type: " << static_cast<int>(m_type) << "\n";
    for(auto& itr: m_hashes) {
        std::cout << cryptor.sha256HashToString(itr) << "\n";
    }
    std::cout << std::endl;
}

BlockMsg::BlockMsg() {
    m_comm = MsgTypes::sBlock;
}

BlockMsg::BlockMsg(const Block& block) {
    m_comm = MsgTypes::sBlock;
    m_block = block;
}

const Block& BlockMsg::getBlock() noexcept {
    return m_block;
}

void BlockMsg::decode(ByteReader& byteReader) {
    decodeHeader(byteReader);
    m_block.decode(byteReader);
}

void BlockMsg::encode(ByteWriter& byteWriter) const {
    encodeHeader(byteWriter, m_block.size(), 0);
    m_block.encode(byteWriter);
}

void BlockMsg::print() const noexcept {
    std::cout << "BlockMsg\n";
    m_block.print();
    std::cout << std::endl;
}

TxMsg::TxMsg() {
    m_comm = MsgTypes::Tx;
}

TxMsg::TxMsg(const Transaction& tx) {
    m_comm = MsgTypes::Tx;
    m_tx = tx;
}

const Transaction& TxMsg::getTransaction() noexcept {
    return m_tx;
}

void TxMsg::decode(ByteReader& byteReader) {
    decodeHeader(byteReader);
    m_tx.decode(byteReader);
}

void TxMsg::encode(ByteWriter& byteWriter) const {
    encodeHeader(byteWriter, m_tx.size(), 0);
    m_tx.encode(byteWriter);
}

void TxMsg::print() const noexcept {
    std::cout << "TxMsg\n";
    m_tx.print();
    std::cout << std::endl;
}

NoFoundMsg::NoFoundMsg() {
    m_comm = MsgTypes::noFound;
}

void NoFoundMsg::decode(ByteReader& byteReader) {
    decodeHeader(byteReader);
}

void NoFoundMsg::encode(ByteWriter& byteWriter) const {
    encodeHeader(byteWriter, 0, 0);
}

void NoFoundMsg::print() const noexcept {
    std::cout << "NoFoundMsg\n";
    std::cout << std::endl;
}
