#ifndef Block_hpp
#define Block_hpp

#include "Transaction.hpp"
#include "sha256.hpp"

#include <array>
#include <ctype.h>
#include <inttypes.h>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string.h>
#include <string>

#define MAXNONCE 1000000

class BlockChain;

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
    ProofOfWork(class Block* block);

    /*!
     \brief Процесс поиска хэша
    */
    void Run();

private:
    std::string PrepareData(); /*!< Данные для хэширования*/

    std::array<uint32_t, 8> m_target; /*!< Массив сравнения хэша*/

    Block* m_block; /*!< Блок для хэширования*/

    uint64_t m_nonce; /*!< Подбираемое случайное число*/
};

/*!
    \brief Блок

    Блок в цепи
*/
class Block {
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
        const std::list<Transaction>& tx,
        const std::array<uint32_t, 8>& prevBlockHash,
        const std::array<uint32_t, 8>& hash,
        const int64_t& nonce);

    /*!
     \brief Конструктор копирования
    */
    Block(Block* block);

    /*!
     \brief Время создания блока
     \return значение в unix формате
   */
    uint64_t getTimeStamp();

    /*!
     \brief Список транзакций
     \return Список указателей
    */
    std::list<Transaction> getTransaction();

    /*!
     \brief Хэш предыдущего блока
     \return массив байт
    */
    std::array<uint32_t, 8> getPrevBlockHash();

    /*!
     \brief Хэш блока
     \return Массив байт
    */
    std::array<uint32_t, 8> getHash();

    /*!
     \brief Подобраное случайное число
     \return число
    */
    uint64_t getNonce();

    /*!
     \brief Задать время создания
     \param [in] timeStamp время создания
    */
    void setTimeStamp(const int64_t& timeStamp);

    /*!
     \brief Задать список транзакций
     \param [in] trans список транзакций
    */
    void setTransaction(std::list<Transaction>& trans);

    /*!
     \brief Задать случайное число
     \param [in] nonce случайное число
    */
    void setNonce(const uint64_t& nonce);

    /*!
     \brief Задать хэш предыдущего блока
     \param [in] hash хэш предыдущего блока
    */
    void setPrevBlockHash(const std::array<uint32_t, 8>& hash);

    /*!
     \brief Задать хэш блока
     \param [in] hash хэш блока
    */
    void setHash(const std::array<uint32_t, 8>& hash);

    /*!
     \brief Найти размер блока
     \return unix ssize_t
   */
    size_t size();

    /*!
     \brief Вывести информацию о блоке в консоль
    */
    void print();

    /*!
     \brief Сериализация блока
     \param [out] ptr - размер блока
    */
    void encode(uint8_t* ptr);

    /*!
    Десериализация блока
    \param [in] ptr массив байтов
    */
    void decode(uint8_t* ptr);

    friend BlockChain;

private:
    int64_t m_timeStamp; /*!< время создания*/

    std::list<Transaction> m_tx; /*!< список транзакций*/

    std::array<uint32_t, 8> m_prevBlockHash; /*!< хэш предыдущего блока*/

    std::array<uint32_t, 8> m_hash; /*!< хэш блока*/

    uint64_t m_nonce; /*!< случайное число*/

    friend ProofOfWork;
};

/*!
 \brief Перевод строку хэша в hex предстовление
 \param [in] arr массив байтов
 \return hex строка
*/
std::string array2String(const std::array<uint32_t, 8>& arr);

void printBigInt(uint32_t* bigint);

#endif
