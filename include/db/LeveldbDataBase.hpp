#ifndef DB_hpp
#define DB_hpp

#include "Block.hpp"
#include "IDataBase.hpp"

#include <leveldb/db.h>

#include <memory>
#include <optional>

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
    void putBlock(const Block& block) override;

    /*!
    \brief Достает последний записанный блок
    \return хэш последнего блока
    */
    std::optional<Hash> getCurrentHash() override;

    /*!
    \brief Достает id последнего блока
    \param [in] hash - хэш блока
    \return id последнего блока
    */
    std::optional<uint64_t> getCurrentId(const Hash& hash) override;

    /*!
    \brief Достает блок по его хэшу
    \param [in] hash - хэш блока
    \return блок  данных
    */
    std::optional<Block> getBlockByHash(const Hash& hash) override;

private:
    std::unique_ptr<leveldb::DB> m_db; /*!< указатель на базу данных*/
};

#endif /* DB_hpp */
