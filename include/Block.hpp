#pragma once

#include "Transaction.hpp"
#include "crypto/Hash.hpp"
#include "crypto/IHashEncoder.hpp"
#include "serialization/ByteReader.hpp"
#include "serialization/ByteWriter.hpp"
#include "serialization/Serializer.hpp"

#include <array>
#include <ctype.h>
#include <inttypes.h>
#include <iomanip>
#include <iostream>
#include <memory>
#include <sstream>
#include <string.h>
#include <string>

#define MAXNONCE 1000000

/*!
    \brief Блок

    Блок в цепи
*/
class Block: public Serializer {
public:
    /*!
     \brief Диструктор
    */
    ~Block();

    /*!
     \brief Конструктор
    */
    Block();
    /*!
     \brief Конструктор с параметрами
     \param [in] timeStamp - время создания блока
     \param [in] tx - список транзакций
     \param [in] prevBlockHash - хэш предыдущего блока
     \param [in] hash - хэш блока
     \param [in] nonce - случайное число
    */
    Block(
        const int64_t& timeStamp,
        const std::list<Transaction>& txs,
        const Hash& prevBlockHash,
        const Hash& hash,
        const int64_t& nonce);

    /*!
     \brief Время создания блока
     \return значение в unix формате
   */
    uint64_t getTimeStamp() const noexcept;

    /*!
     \brief Список транзакций
     \return Список указателей
    */
    const std::list<Transaction>& getTransactions() const noexcept;

    /*!
     \brief Хэш предыдущего блока
     \return массив байт
    */
    const Hash& getPrevBlockHash() const noexcept;

    /*!
     \brief Хэш блока
     \return Массив байт
    */
    const Hash& getHash() const noexcept;

    /*!
     \brief Подобраное случайное число
     \return число
    */
    uint64_t getNonce() const noexcept;

    /*!
     \brief Задать время создания
     \param [in] timeStamp время создания
    */
    void setTimeStamp(const int64_t& timeStamp) noexcept;

    /*!
     \brief Задать список транзакций
     \param [in] trans список транзакций
    */
    void setTransaction(const std::list<Transaction>& trans);

    /*!
     \brief Задать случайное число
     \param [in] nonce случайное число
    */
    void setNonce(const uint64_t& nonce) noexcept;

    /*!
     \brief Задать хэш предыдущего блока
     \param [in] hash хэш предыдущего блока
    */
    void setPrevBlockHash(const Hash& hash) noexcept;

    /*!
     \brief Задать хэш блока
     \param [in] hash хэш блока
    */
    void setHash(const Hash& hash) noexcept;

    /*!
     \brief Найти размер блока
     \return unix ssize_t
   */
    size_t size() const noexcept;

    /*!
     \brief Вывести информацию о блоке в консоль
    */
    void print() const noexcept;

    /*!
     \brief Сериализация блока
     \param [out] ptr - размер блока
    */
    void encode(ByteWriter& writer) const override;

    /*!
    Десериализация блока
    \param [in] ptr массив байтов
    */
    void decode(ByteReader& reader) override;

private:
    int64_t m_timeStamp;

    std::list<Transaction> m_tx;

    Hash m_prevBlockHash;

    Hash m_hash;

    uint64_t m_nonce;
};

/*!
    \brief Доказательство работы

    Поиск хэша блока с учетом правил
*/
class ProofOfWork {
public:
    /*!
     \brief Конструктор
     \param [in] block - блок для хэширования
    */
    ProofOfWork(Block& block);

    /*!
     \brief Процесс поиска хэша
    */
    void Run();

private:
    bool hashIsValid(const Hash& hash);

    std::string PrepareData(); /*!< Данные для хэширования*/

    Block& m_block; /*!< Блок для хэширования*/

    uint64_t m_nonce; /*!< Подбираемое случайное число*/
};
