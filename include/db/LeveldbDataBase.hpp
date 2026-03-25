#ifndef DB_hpp
#define DB_hpp

#include "Block.hpp"
#include "IDataBase.hpp"
#include <leveldb/db.h>

#include <memory>

/*!
    \brief DAO для работы с базой данных leveldb
*/
class LeveldbDataBase: public IDataBase {
public:
    /*!
    \brief Открытие базы данных
    */
    void connect() override;

    /*!
    \brief Запись блока в базу данных
    \param [in] block - блок данных
    */
    void putBlock(const std::unique_ptr<Block>& block) override;

    /*!
    \brief Достает последний записанный блок
    \return хэш последнего блока
    */
    std::array<uint8_t, 32> getCurrentHash() override;

    /*!
    \brief Достает id последнего блока
    \param [in] hash - хэш блока
    \return id последнего блока
    */
    uint64_t getCurrentId(const std::array<uint8_t, 32>& hash) override;

    /*!
    \brief Достает блок по его хэшу
    \param [in] hash - хэш блока
    \return блок  данных
    */
    std::unique_ptr<Block> getBlockByHash(const std::array<uint8_t, 32>& hash) override;

private:
    std::unique_ptr<leveldb::DB> m_db; /*!< указатель на базу данных*/
};

#endif /* DB_hpp */
