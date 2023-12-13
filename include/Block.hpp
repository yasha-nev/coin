#ifndef Block_hpp
#define Block_hpp

#include <string>
#include <sstream>
#include <iomanip>
#include <ctype.h>
#include <string.h>
#include <array>

#include <inttypes.h>
#include <iostream>
#include "sha256.hpp"
#include "Transaction.hpp"

#define MAXNONCE 1000000

class BlockChain;

/*!
    \brief Докалательство работы

    Поиск хэша блока
*/
class ProofOfWork{
public:
    /*!
    конструктор
    \param [in] block - блок для хэширования
    */
    ProofOfWork(class Block *block);
    
    /*!
    Процесс поиска хэша
    */
    void Run();
    
private:
    std::string PrepareData();
    
    std::array<uint32_t, 8> m_target;
    
    Block *m_block;
    
    uint64_t m_nonce;
};

/*!
    \brief Блок

    Блок в цепи
*/
class Block{
public:

    /*!
    Диструктор
    */
    ~Block();
    
    /*!
    Конструктор
    */
    Block();
    /*!
    Конструктор с параметрами
    \param [in] timeStamp - время создания блока
    \param [in] tx - список транзакций
    \param [in] prevBlockHash - хэш предыдущего блока
    \param [in] hash - хэш блока
    \param [in] nonce - случайное число
    */
    Block(const int64_t &timeStamp,
          std::list<Transaction *>tx,
          const std::array<uint32_t, 8> &prevBlockHash,
          const std::array<uint32_t, 8> &hash,
          const int64_t &nonce);
    
     /*!
    \return время создания
    */
    uint64_t getTimeStamp();
    
    /*!
    \return список транзакций
    */
    std::list<Transaction *> getTransaction();
    
    /*!
    \return хэш предыдущего блока
    */
    std::array<uint32_t, 8> getPrevBlockHash();
    
    /*!
    \return хэш блока
    */
    std::array<uint32_t, 8> getHash();
    
    /*!
    \return случайное число
    */
    uint64_t getNonce();
    
    /*!
    Установить время создания
    \param [in] timeStamp время создания
    */
    void setTimeStamp(const int64_t &timeStamp);
    
    /*!
    Установить список транзакций
    \param [in] trans список транзакций
    */
    void setTransaction(std::list<Transaction *>trans);
    
    void setTransaction(Transaction *trans);
    
     /*!
    Установить случайное число
    \param [in] nonce случайное число
    */
    void setNonce(const uint64_t &nonce);
    
     /*!
    Установить хэш предыдущего блока
    \param [in] hash хэш предыдущего блока
    */
    void setPrevBlockHash(const std::array<uint32_t, 8> &hash);
    
     /*!
    Установить хэш блока
    \param [in] hash хэш блока
    */
    void setHash(const std::array<uint32_t, 8> &hash);
    
     /*!
    \return размер блока
    */
    size_t size();
    
    /*!
    Вывести блок в консоль
    */
    void print();
    
     /*!
    Сериализация блока
    \param [out] blockSize - размер блока
    \return сериализованный массив байтов
    */
    uint8_t *encode(size_t *blockSize);
    
    friend BlockChain;
    
private:
    int64_t m_timeStamp; /*!< время создания*/
    
    std::list<Transaction *> m_tx; /*!< список транзакций*/
    
    std::array<uint32_t, 8> m_prevBlockHash; /*!< хэш предыдущего блока*/
    
    std::array<uint32_t, 8> m_hash; /*!< хэш блока*/
    
    uint64_t m_nonce; /*!< случайное число*/
    
    friend ProofOfWork;
    
    friend Block *decode(uint8_t *dec);
};

/*!
Десериализация блока
\param [in] dec массив байтов
\return блок
*/
Block *decode(uint8_t *dec);

std::string array2String(const std::array<uint32_t, 8> &arr);

void printBigInt(uint32_t *bigint);

#endif
