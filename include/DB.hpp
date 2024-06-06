#ifndef DB_hpp
#define DB_hpp

#include "Block.hpp"
#include "leveldb/db.h"
#include <memory>

#define DBPATH "./testdb"

/*!
    \brief DAO для работы с базой данных leveldb
*/
class DB {
public:

    /*!
    \brief Конструктор
    */
    DB();
    
    /*!
    \brief Диструктор
    */
    ~DB();
    
    /*!
    \brief Создание базы данных
    */
    void connect();
    
    /*!
     \brief Открытие базы данных
    */
    void connectIfexist();
    
    /*!
    \brief Запись блока в базу данных
    \param [in] block - блок данных
    */
    void putBlock(std::unique_ptr<Block> &block);

    /*!
    \brief Достает последний записанный блок
    \return хэш последнего блока
    */
    std::string getCurrentHash();
    
    /*!
    \brief Достает id последнего блока
    \param [in] hash - хэш блока
    \return id последнего блока
    */
    uint64_t getCurrentId(const std::array<uint32_t, 8> &hash);
    
    /*!
    \brief Достает блок по его хэшу
    \param [in] hash - хэш блока
    \return блок  данных
    */
    std::unique_ptr<Block> getBlockByHash(const std::array<uint32_t, 8> &hash);
    
    leveldb::DB* m_db;  /*!< указатель на базу данных*/
};

#endif /* DB_hpp */
