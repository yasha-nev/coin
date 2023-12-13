#ifndef Message_hpp
#define Message_hpp

#define STARTSTR "f9beb4d9"

#include <inttypes.h>
#include <string.h>
#include <list>
#include <array>
#include "Block.hpp"
#include "sha256.hpp"

/*!
    \brief Типы сообщений
*/
enum MsgTypes{
    gBlocks = 0, /*!< getBlock message */
    gData   = 1, /*!< getData message */
    sBlock  = 2, /*!< Block message */
    Inv     = 3, /*!< INV message */
    Tx      = 4, /*!< TX message */
    noFound = 5  /*!< noFound message*/
};

/*!
    \brief Базовый класс сообщения
*/
class Message{
public:
    /*!
    Виртуальный диструктор
    */
    virtual ~Message();

    /*!
    Десериализация
    \param [in] data - исходный массив байт
    \param [in] size - размер сообщения
    */
    virtual void parse(uint8_t *data, size_t size) = 0;
    
    /*!
    Сериализация
    \param [out] size - размер сообщения
    \return массив байт
    */
    virtual uint8_t *toByte(size_t &size) = 0;
    
    /*!
    \return тип сообщения
    */
    uint8_t getCommand();
    
    /*!
    Задать id клиента
    \param [in] id - id клиента
    */
    void setClientId(int id);
    
    /*!
    \return id клиента
    */
    int getClientId();
    
protected:
    int m_clientId = -1; /*!< id клиента*/
    
    uint8_t m_ver = 0; /*!< версия*/
    
    size_t m_size; /*!< размер сообщения*/
    
    uint8_t m_comm; /*!< тип сообщения*/
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
    \brief Block message
    Сообщения для запроса блоков у другого узла

    потомок Message
*/
class GetBlocksMsg : public Message{
public:
    /*!
    Конструктор
    */
    GetBlocksMsg();

    /*!
    Конструктор с параметрами
    \param hashes - хэши блоков
    */
    GetBlocksMsg(std::list<std::array<uint32_t, 8>> hashes);
    
    /*!
    Десериализация
    \param [in] data - исходный массив байт
    \param [in] size - размер сообщения
    */
    virtual void parse(uint8_t *data, size_t size) override;
    
    /*!
    Сериализация
    \param [out] size - размер сообщения
    \return массив байт
    */
    virtual uint8_t *toByte(size_t &size) override;
    
    /*!
    \return список хэшей
    */
    std::list<std::array<uint32_t, 8>> getHashes();

private:
    std::list<std::array<uint32_t, 8>> m_hashes; /*!< список хэшей */
    
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
enum InvTypes{
    iBlock = 0, /*!< Блок */
    iTx = 1 /*!< Транзакция */
};

/*!
    \brief INV message
    Ответ на getBlock сообщение

    потомок Message
*/
class InvMsg : public Message{
public:
    /*!
    Конструктор
    */
    InvMsg();
    
    /*!
    Конструктор с параметрами
    \param [in] type - тип сериализованных данных
    \param [in] hashes - список хэшей блоков
    */
    InvMsg(InvTypes type, std::list<std::array<uint32_t, 8>> hashes);
    
    /*!
    Десериализация
    \param [in] data - исходный массив байт
    \param [in] size - размер сообщения
    */
    virtual void parse(uint8_t *data, size_t size) override;
    
    /*!
    Сериализация
    \param [out] size - размер сообщения
    \return массив байт
    */
    virtual uint8_t *toByte(size_t &size) override;
    
    /*!
    \return список хэшей
    */
    std::list<std::array<uint32_t, 8>> getHashes();

private:
    
    uint8_t m_type; /*!< Тип хранимых данных */
    
    std::list<std::array<uint32_t, 8>> m_hashes; /*!< список хэшей */
    
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
enum DataTypes{
    dBlock = 0,
    dTx = 1
};

/*!
    \brief GetData message
    Запрос на получение информации от другого узла

    потомок Message
*/
class GetDataMsg : public Message{
public:
    /*!
    Конструктор
    */
    GetDataMsg();
    
    /*!
    Конструктор с параметрами
    \param [in] type - тип сериализованный данных
    */
    GetDataMsg(DataTypes type, std::list<std::array<uint32_t, 8>> hashes);
    
    /*!
    Десериализация
    \param [in] data - исходный массив байт
    \param [in] size - размер сообщения
    */
    virtual void parse(uint8_t *data, size_t size) override;
    
    /*!
    Сериализация
    \param [out] size - размер сообщения
    \return массив байт
    */
    virtual uint8_t *toByte(size_t &size) override;
    
    /*!
    \return список хэшей
    */
    std::list<std::array<uint32_t, 8>> getHashes();

private:
    uint8_t m_type; /*!< тип хранимых данных */

    std::list<std::array<uint32_t, 8>> m_hashes; /*!< список хэшей */
    
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
    \brief Block message
    Хранит блок цепи

    потомок Message
*/
class BlockMsg : public Message{
public:
    /*!
    Конструктор
    */
    BlockMsg();
    
    /*!
    Конструктор с параметром
    \param [in] blocks - блок из цепи
    */
    BlockMsg(Block *blocks);
    
    /*!
    Десериализация
    \param [in] data - исходный массив байт
    \param [in] size - размер сообщения
    */
    virtual void parse(uint8_t *data, size_t size) override;
    
    /*!
    Сериализация
    \param [out] size - размер сообщения
    \return массив байт
    */
    virtual uint8_t *toByte(size_t &size) override;
    
    /*!
    \return блок из цепи
    */
    Block *getBlock();

private:
    Block *m_block; /*!< Блок из цепи */
    
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

class TxMsg : public Message{
public:
    
    TxMsg();
    
    TxMsg(Transaction *tx);
    
    virtual void parse(uint8_t *data, size_t size) override;
    
    virtual uint8_t *toByte(size_t &size) override;
    
    Transaction *getTransaction();

private:
    Transaction *m_tx;
    
};


/*               Message Header
 *  ------------------------------------------
 * | start_string | Command | Size | Checksum |
 *  ------------------------------------------
 */

/*!
    \brief NoFoundMsg message
    Ответ в случае ошибки

    потомок Message
*/
class NoFoundMsg : public Message{
public:
    /*!
    Конструктор
    */
    NoFoundMsg();
    
    /*!
    Десериализация
    \param [in] data - исходный массив байт
    \param [in] size - размер сообщения
    */
    virtual void parse(uint8_t *data, size_t size) override;
    
    /*!
    Сериализация
    \param [out] size - размер сообщения
    \return массив байт
    */
    virtual uint8_t *toByte(size_t &size) override;

};

#endif
