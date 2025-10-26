#ifndef BlockChain_hpp
#define BlockChain_hpp

#include "Block.hpp"
#include "IDataBase.hpp"

#include <array>
#include <list>
#include <memory>
#include <vector>

/*!
    \brief Цепь блоков
*/
class BlockChain {
public:
    /*!
     \brief Конструктор
    */
    BlockChain(std::unique_ptr<IDataBase> db);

    /*!
     \brief Вывод цепи блоков в консоль
    */
    void printChain() const noexcept;

    /*!
     \brief Подожить блок в базе данных
     \param [in] block указатель на блок
    */
    void putBlock(std::unique_ptr<Block>& block);

    /*!
     \brief Создать блок из списка транзакций
     \param [in] time - время создания блока
     \param [in] tx - список транзакций
    */
    void createBlock(uint64_t time, const std::list<Transaction>& tx);

    /*!
     \brief Получить id последней транзакции
     \return id транзакции
    */
    uint64_t getPastTransactionId() const noexcept;

    /*!
     \brief Сумма денег клиента в блокчейне
     \return Сумма
    */
    uint64_t getBalance(const std::string& pubkey, const std::string& address) const noexcept;

    /*!
     \brief Получить блок по хэшу
     \param [in] hash - хэш блока
     \return блок
    */
    std::unique_ptr<Block> getBlock(const std::array<uint32_t, 8>& hash) const noexcept;

    /*!
     \brief Получить последний записанный блок
     \return Блок
    */
    std::unique_ptr<Block> getPastBlock() const noexcept;

    /*!
     \brief Получить хэш последнего блока
     \return хэш
    */
    const std::array<uint32_t, 8>& getPastBlockHash() const noexcept;

    /*!
     \brief Возвращает хэши блоков пока не дайдет до заданого, иначе все хэши
     \param [in] curHash - хэш блока
     \return список блоков
    */
    std::list<std::array<uint32_t, 8>> getHashesBefore(
        std::array<uint32_t, 8> curHash) const noexcept;

    /*!
     \brief Создание списка неиспользованных выходов
     \param [in] address - адрес кошелька
     \param [in] value - нужная сумма
     \param [in] rest - размер сдачи
     \return список входов
    */
    std::list<TXInput> getInputs(
        const std::string& pubkey,
        const std::string& address,
        int value,
        int* rest) const noexcept;

private:
    /*!
     \brief Создание нового блока
     \param [in] time - текущее время
     \param [in] tx - список транзакций
     \param [in] hash - хэш предыдущего блока
     \return указатель на созданный блок
    */
    std::unique_ptr<Block>
    newBlock(uint64_t time, const std::list<Transaction>& tx, const std::array<uint32_t, 8>& hash);

    /*!
     \brief Создание базового первого блока
     \return указатель на созданный блок
    */
    std::unique_ptr<Block> genesisBlock();

    std::array<uint32_t, 8> m_cur_hash; /*!< хэш последнего блока*/

    std::unique_ptr<IDataBase> m_db; /*!< dao базы данных*/
};

/*!
 \brief Проверка файла на существования
 \param [in] path - Путь к файлу
 \return результат поиска
*/
bool file_exist(const std::string& path);

#endif
