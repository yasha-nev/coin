#ifndef Transaction_hpp
#define Transaction_hpp

#include <inttypes.h>
#include <string>
#include <string.h>
#include <sstream>
#include <iomanip>
#include <iostream>
#include <list>


#define REWARD 50

/*!
    \brief Транзакция

    Является частью блока
*/
class Transaction{
public:
    /*!
    Базовый конструктор
    */
    uint64_t m_id; /*!< id транзакции */
    
    int m_inCount; /*!< Количество входов */
    
    int m_outCount; /*!< Количество выходов */
    
    class TXInput *m_in; /*!< Входы */
    
    class TXOutput *m_out; /*!< Выходы */
    
    /*!
    Конструктор
    \param [in] id - id транзакции
    \param [in] in_cout - количество входов
    \param [in] out_count - количество выходов
    */
    Transaction(uint64_t id, int in_cout, int out_count);
    
    /*!
    Конструктор копирования
    */
    Transaction(Transaction *tx);
    
    /*!
    Диструктор
    */
    ~Transaction();

    /*!
    Переводид поля транзакции в строку
    */
    std::string toString();

    /*!
    Размер транзакции
    */
    size_t size();

    /*!
    Сериализация
    \param [in] ptr - массив байт для записи
    */
    void encode(uint8_t *ptr);
    
    /*!
    Десериализация
    \param [in] ptr - массив байт для чтения
    */
    void decode(uint8_t *ptr);
    
    void print();
};

/*!
    \brief Выход

    Элемент Транзакции
*/
class TXOutput{
public:
    /*!
    Конструктор
    */
    TXOutput(){};
    
    /*!
    Конструктор копирования
    */
    TXOutput(const TXOutput &out);
    
    /*!
    Конструктор с параметрами

    \param [in] value - количество передоваемых монет
    \param [in] pubkey - адрес кошелька
    */
    TXOutput(int value, const std::string &pubkey);
    
    /*!
    Перегрузка =
    */
    TXOutput &operator =(const TXOutput &out);
    
    int m_value; /*!< количество монет */
    
    std::string m_address; /*!< адрес кошелька */
};

/*!
    \brief Вход

    Элемент Транзакции
*/
class TXInput{
public:
    /*!
    Конструктор
    */
    TXInput(){};
    
    /*!
    Конструктор копирования
    */
    TXInput(const TXInput &in);
    
    /*!
    Конструктор с параметрами

    \param [in] transId - id транзакции
    \param [in] outIndex - индекс выхода в транзакции
    \param [in] pubkey - публичный ключ отправителя
    */
    TXInput(const uint64_t &transId, int outIndex, const std::string &pubkey);

    /*!
    Перегрузка =
    */
    TXInput& operator =(const TXInput &in);
    
    uint64_t m_tranId; /*!< id транзакции */
    
    int m_outIndex; /*!< индекс выхода в транзакции*/
    
    std::string m_pubkey; /*!< публичный ключ */

    std::string m_sign; /*!< цифровая подпись */
};

/*!
Вознограждение
\param [in] id - id транзакции
\param [in] address - адрес кошелька
*/
Transaction *coinBaseTrans(const uint64_t &id, const std::string &address);

/*!
Вознограждение

\param [in] id - id транзакции
\param [in] from - публичный ключ отправителя
\param [in] to - адрес получателя
\param [in] value - сумма перевода
\param [in] inputs - список выходов
\param [in] rest - размер сдачи
*/
Transaction *realTransaction(const uint64_t &id, const std::string &from, const std::string &to, int value, std::list<TXInput> inputs, int rest);

#endif /* Transaction_hpp */
