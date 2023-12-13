#ifndef BlockChain_hpp
#define BlockChain_hpp

#include "DB.hpp"
#include "Block.hpp"
#include <list>
#include <array>

/*!
    \brief Цепь блоков
*/
class BlockChain{
public:

    /*!
    конструктор
    */
    BlockChain();
    
    /*!
    Напечатать цепь блоков
    */
    void printChain();
    
    /*!
    Сохранить блок в базе данных
    \param [in] block указатель на блок
    */
    void putBlock(Block *block);
    
    /*!
    Создать блок из списка транзакций
    \param [in] time - время создания блока
    \param [in] tx - список транзакций
    */
    void createBlock(uint64_t time, std::list<Transaction *> tx);
    
    /*!
    Получить id последней транзакции
    \return id транзакции
    */
    uint64_t getPastTransactionId();
    /*!
    \return сумма денег в кошельке
    */
    uint64_t getBalance(const std::string &pubkey, const std::string &address);
    
    /*!
     Получить блок по хэшу
    \param [in] hash - хэш блока
    \return блок
    */
    Block *getBlock(const std::array<uint32_t, 8> &hash);
    
    /*!
    \return последний записанный блок
    */
    Block *getPastBlock();
    
    /*!
    \return хэш последнего записаного блока
    */
    std::array<uint32_t, 8> getPastBlockHash();
    
    /*!
    \param [in] curHash - хэш блока
    \return список всех блоков перед данным
    */
    std::list<std::array<uint32_t, 8>> getHashesBefore(std::array<uint32_t, 8> curHash);
    
    /*!
    Создает список неиспользованных выходов
    \param [in] address - адрес кошелька
    \param [in] value - нужная сумма
    \param [in] rest - размер сдачи
    \return список входов
    */
    std::list<TXInput> getInputs(const std::string &pubkey, const std::string &address, int value, int *rest);
    
private:
    
    /*!
    Создает новый блок
    \param [in] time - текущее время
    \param [in] tx - список транзакций
    \param [in] hash - хэш предыдущего блока
    \return указатель на созданный блок
    */
    Block *newBlock(uint64_t time, std::list<Transaction *> tx, const std::array<uint32_t, 8> &hash);
    
    /*!
    Создает базовый блока
    \return указатель на созданный блок
    */
    Block *genesisBlock();
    
    std::array<uint32_t, 8> m_cur_hash; /*!< хэш последнего блока*/
    
    DB m_db; /*!< dao базы данных*/
};

bool file_exist (const std::string& path);

#endif
