#ifndef DB_hpp
#define DB_hpp

#include "Block.hpp"
#include "leveldb/db.h"

#define DBPATH "./testdb"

/*!
    \brief DAO для работы с базой данных
*/
class DB {
public:

    /*!
    конструктор
    */
    DB();
    
    /*!
    диструктор
    */
    ~DB();
    
    /*!
    Создание базы данных
    */
    void connect();
    
    /*!
    Открытие базы данных
    */
    void connectIfexist();
    
    /*!
    Положить блок
    \param [in] block - блок данных
    */
    void putBlock(Block *block);

    /*!
    \return хэш последнего блока
    */
    std::string getCurrentHash();
    
    /*!
    \param [in] hash - хэш блока
    \return id последнего блока
    */
    uint64_t getCurrentId(const std::array<uint32_t, 8> &hash);
    
    /*!
    \param [in] hash - хэщ блока
    \return блок  данных
    */
    Block *getBlockByHash(const std::array<uint32_t, 8> &hash);
    
    leveldb::DB* m_db;  /*!< указатель на базу данных*/
};

#include <stdio.h>

#endif /* DB_hpp */
