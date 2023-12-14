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
    Хранит информацию о переводах средств
*/
class Transaction{
public:
    /*!
    \brief Базовый конструктор
    */
    uint64_t m_id; /*!< id транзакции */
    
    int m_inCount; /*!< Количество входов */
    
    int m_outCount; /*!< Количество выходов */
    
    class TXInput *m_in; /*!< Входы */
    
    class TXOutput *m_out; /*!< Выходы */
    
    /*!
    \brief Конструктор с параметрами
    \param [in] id - id транзакции
    \param [in] in_cout - количество входов
    \param [in] out_count - количество выходов
    */
    Transaction(uint64_t id, int in_cout, int out_count);
    
    /*!
    \brief Конструктор копирования
    */
    Transaction(Transaction *tx);
    
    /*!
    \brief Диструктор
    */
    ~Transaction();

    /*!
    \brief Парсит поля транзакции в строку
    */
    std::string toString();

    /*!
    \brief Размер транзакции
    */
    size_t size();

    /*!
    \brief Сериализация транзакции
    \param [in] ptr - массив байт для записи
    */
    void encode(uint8_t *ptr);
    
    /*!
    \brief Десериализация транзакции
    \param [in] ptr - массив байт для чтения
    */
    void decode(uint8_t *ptr);
    
    /*!
    \brief Вывод информации о транзакции в консоль
    */
    void print();
};

/*!
    \brief Выход транзакции

     Хранит информацию куда был сделал перевод
*/
class TXOutput{
public:
    /*!
     \brief Конструктор
    */
    TXOutput(){};
    
    /*!
     \brief Конструктор копирования
    */
    TXOutput(const TXOutput &out);
    
    /*!
     \brief Конструктор с параметрами

     \param [in] value - количество передоваемых монет
     \param [in] pubkey - адрес кошелька
    */
    TXOutput(int value, const std::string &pubkey);
    
    /*!
     \brief Перегрузка оператора  =
    */
    TXOutput &operator =(const TXOutput &out);
    
    int m_value; /*!< количество монет */
    
    std::string m_address; /*!< адрес кошелька */
};

/*!
    \brief Вход транзакции

    Хранит информацию откуда были взяты средства
    Является частью Транзакции
*/
class TXInput{
public:
    /*!
     \brief Конструктор
    */
    TXInput(){};
    
    /*!
     \brief Конструктор копирования
    */
    TXInput(const TXInput &in);
    
    /*!
     \brief Конструктор с параметрами

    \param [in] transId - id транзакции
    \param [in] outIndex - индекс выхода в транзакции
    \param [in] pubkey - публичный ключ отправителя
    */
    TXInput(const uint64_t &transId, int outIndex, const std::string &pubkey);

    /*!
     \brief Перегрузка =
    */
    TXInput& operator =(const TXInput &in);
    
    uint64_t m_tranId; /*!< id транзакции */
    
    int m_outIndex; /*!< индекс выхода в транзакции*/
    
    std::string m_pubkey; /*!< публичный ключ */

    std::string m_sign; /*!< цифровая подпись */
};

/*!
 \brief Вознограждение
 \param [in] id - id транзакции
 \param [in] address - адрес кошелька
*/
Transaction *coinBaseTrans(const uint64_t &id, const std::string &address);

/*!
 \brief Создание обычной транзакции

 \param [in] id - id транзакции
 \param [in] from - публичный ключ отправителя
 \param [in] to - адрес получателя
 \param [in] value - сумма перевода
 \param [in] inputs - список выходов
 \param [in] rest - размер сдачи
*/
Transaction *realTransaction(const uint64_t &id, const std::string &from, const std::string &to, int value, std::list<TXInput> inputs, int rest);

#endif /* Transaction_hpp */
