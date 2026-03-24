#ifndef Message_hpp
#define Message_hpp

#define STARTSTR "f9beb4d9"

#include "Block.hpp"
#include "ByteReader.hpp"
#include "ByteWriter.hpp"
#include "CryptoppImpl.hpp"
#include "Hash.hpp"
#include "Serializer.hpp"

#include <array>
#include <inttypes.h>
#include <list>
#include <memory>
#include <sstream>
#include <string.h>

/*!
    \brief Типы сообщений
*/
enum class MsgType : uint8_t {
    gBlocks = 0, /*!< getBlock message */
    gData = 1,   /*!< getData message */
    sBlock = 2,  /*!< Block message */
    Inv = 3,     /*!< INV message */
    Tx = 4,      /*!< TX message */
    noFound = 5  /*!< noFound message*/
};

#define START_STRING "f9beb4d9"
#define START_STRING_SIZE 9
#define CHECK_SUM_SIZE 33

struct MessageHeader {
    std::array<char, START_STRING_SIZE> startString;
    MsgType command;
    size_t payloadSize;
    std::array<char, CHECK_SUM_SIZE> checkSum;
};

/*!
    \brief Базовый класс сообщения
*/
class Message: public Serializer {
public:
    /*!
     \brief Виртуальный диструктор
    */
    virtual ~Message();

    virtual void encode(ByteWriter& byteWriter) const override = 0;

    virtual void decode(ByteReader& byteReader) override = 0;

    void encodeHeader(ByteWriter& byteWriter) const;

    void decodeHeader(ByteReader& byteReader);

    /*!
     \brief Тип message
     \return байт
    */
    MsgType getCommand() noexcept;

    /*!
     \brief заголовок сообщения
     \return MessageHeader
    */
    const MessageHeader& getHeader();

    /*!
     \brief Информация хронящаяся в сообщении
     */
    virtual void print() const noexcept = 0;

protected:
    MessageHeader m_header;

    size_t m_size; /*!< размер сообщения*/
};

/*               Message Header
 *  ------------------------------------------
 * | start_string | Command | Size | Checksum |
 *  ------------------------------------------
 *                   Payload
 *  --------------------------------------------
 * | Prot vers         |    Header Hash Count 1 |
 * |--------------------------------------------|
 * | Header heash                               |
 * | Stopping hash 00000000000000000000000000000|
 *  --------------------------------------------
 */

/*!
    \brief Block Сообщение
    Сообщения для запроса блоков у другого узла

    потомок Message
*/
class GetBlocksMsg: public Message {
public:
    /*!
     \brief Конструктор
    */
    GetBlocksMsg();

    /*!
     \brief Конструктор с параметрами
     \param hashes - хэши блоков
    */
    GetBlocksMsg(const std::list<Hash>& hashes);

    virtual void encode(ByteWriter& byteWriter) const override;

    virtual void decode(ByteReader& byteReader) override;

    /*!
     \brief Информация хронящаяся в сообщении
     */
    virtual void print() const noexcept override;

    /*!
    \return список хэшей
    */
    const std::list<Hash>& getHashes() noexcept;

private:
    uint8_t m_version = 0;

    std::list<Hash> m_hashes; /*!< список хэшей */
};

/*               Message Header
 *  ------------------------------------------
 * | start_string | Command | Size | Checksum |
 *  ------------------------------------------
 *                   Payload
 *  --------------------------------------------
 * |           number of inv(500)               |
 * |--------------------------------------------|
 * | type        unique identif(header hash)    |
 * | block         header hash                  |
 * | block         header hash                  |
 *  --------------------------------------------
 */

/*!
    \brief типы данных в INV
*/
enum class InvTypes : uint8_t {
    iBlock = 0, /*!< Блок */
    iTx = 1     /*!< Транзакция */
};

/*!
    \brief INV Сообщение
    Ответ на getBlock сообщение

    потомок Message
*/
class InvMsg: public Message {
public:
    /*!
     \brief Конструктор
    */
    InvMsg();

    /*!
     \brief Конструктор с параметрами
     \param [in] type - тип сериализованных данных
     \param [in] hashes - список хэшей блоков
    */
    InvMsg(InvTypes type, const std::list<Hash>& hashes);

    virtual void encode(ByteWriter& byteWriter) const override;

    virtual void decode(ByteReader& byteReader) override;

    /*!
     \brief Информация хронящаяся в сообщении
     */
    virtual void print() const noexcept override;

    /*!
    \return список хэшей
    */
    const std::list<Hash>& getHashes() noexcept;

private:
    InvTypes m_type; /*!< Тип хранимых данных */

    std::list<Hash> m_hashes; /*!< список хэшей */
};

/*               Message Header
 *  ------------------------------------------
 * | start_string | Command | Size | Checksum |
 *  ------------------------------------------
 *                   Payload
 *  --------------------------------------------
 * |           number of objects(128)           |
 * |--------------------------------------------|
 * | type        unique identif(header hash)    |
 * | block         header hash                  |
 * | block         header hash                  |
 *  --------------------------------------------
 */

/*!
    \brief типы данных в getData сообщении
*/
enum class DataTypes : uint8_t { dBlock = 0, dTx = 1 };

/*!
    \brief GetData Сообщение
    Запрос на получение информации от другого узла

    потомок Message
*/
class GetDataMsg: public Message {
public:
    /*!
     \brief Конструктор
    */
    GetDataMsg();

    /*!
     \brief Конструктор с параметрами
    \param [in] type - тип сериализованный данных
    */
    GetDataMsg(DataTypes type, const std::list<Hash>& hashes);

    virtual void encode(ByteWriter& byteWriter) const override;

    virtual void decode(ByteReader& byteReader) override;

    /*!
     \brief Информация хронящаяся в сообщении
     */
    virtual void print() const noexcept override;

    /*!
    \return список хэшей
    */
    const std::list<Hash>& getHashes() noexcept;

private:
    DataTypes m_type; /*!< тип хранимых данных */

    std::list<Hash> m_hashes; /*!< список хэшей */
};

/*               Message Header
 *  ------------------------------------------
 * | start_string | Command | Size | Checksum |
 *  ------------------------------------------
 *                   Payload
 *  --------------------------------------------
 * |              serilized block               |
 *  --------------------------------------------
 */

/*!
    \brief Block Сообщение
    Служит для передачи блока цепи между узлами сети

    потомок Message
*/
class BlockMsg: public Message {
public:
    /*!
     \brief Конструктор
    */
    BlockMsg();

    /*!
     \brief Конструктор с параметром
     \param [in] blocks - блок из цепи
    */
    BlockMsg(const Block& blocks);

    virtual void encode(ByteWriter& byteWriter) const override;

    virtual void decode(ByteReader& byteReader) override;

    /*!
     \brief Информация хронящаяся в сообщении
     */
    virtual void print() const noexcept override;

    /*!
     \return блок из цепи
    */
    const Block& getBlock() noexcept;

private:
    Block m_block; /*!< Блок из цепи */
};

/*               Message Header
 *  ------------------------------------------
 * | start_string | Command | Size | Checksum |
 *  ------------------------------------------
 *                   Payload
 *  --------------------------------------------
 * |              serilized block               |
 *  --------------------------------------------
 */

/*!
    \brief Tx Сообщение

    Служит для отправки транзакций между узлами сети
    Потомок Message
*/
class TxMsg: public Message {
public:
    TxMsg();

    TxMsg(const Transaction& tx);

    virtual void encode(ByteWriter& byteWriter) const override;

    virtual void decode(ByteReader& byteReader) override;

    /*!
     \brief Информация хронящаяся в сообщении
     */
    virtual void print() const noexcept override;

    const Transaction& getTransaction() noexcept;

private:
    Transaction m_tx;
};

/*               Message Header
 *  ------------------------------------------
 * | start_string | Command | Size | Checksum |
 *  ------------------------------------------
 */

/*!
    \brief NoFoundMsg Сообщение
    Ответ в случае ошибки

    потомок Message
*/
class NoFoundMsg: public Message {
public:
    /*!
    Конструктор
    */
    NoFoundMsg();

    virtual void encode(ByteWriter& byteWriter) const override;

    virtual void decode(ByteReader& byteReader) override;

    /*!
     \brief Информация хронящаяся в сообщении
     */
    virtual void print() const noexcept override;
};

#endif
