#ifndef Transaction_hpp
#define Transaction_hpp

#include "serialization/ByteWriter.hpp"
#include "serialization/ByteReader.hpp"
#include "serialization/Serializer.hpp"

#include <Crypto/CryptoppImpl.hpp>

#include "TXInput.hpp"
#include "TXOutput.hpp"

#include <inttypes.h>
#include <iomanip>
#include <iostream>
#include <list>
#include <sstream>
#include <string.h>
#include <string>

#include <vector>

#define REWARD 50

/*!
    \brief Транзакция

    Является частью блока
    Хранит информацию о переводах средств
*/
class Transaction : public Serializer{
public:

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
    Transaction(Transaction* tx);

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
    size_t size() const noexcept;

    /*!
    \brief Сериализация транзакции
    \param [in] ptr - массив байт для записи
    */
    std::vector<std::byte> encode() const override;

    /*!
    \brief Десериализация транзакции
    \param [in] ptr - массив байт для чтения
    */
    void decode(const std::vector<std::byte> &data) override;

    /*!
    \brief Вывод информации о транзакции в консоль
    */
    void print() const noexcept;

    bool operator== (const Transaction &tx) const;

    uint64_t getId() const noexcept;

    const std::vector<TXInput> &getInputs() const noexcept;

    const std::vector<TXOutput> &getOutputs() const noexcept;

    void sign();

protected:
    uint64_t m_id;

    std::vector<TXInput> m_in;

    std::vector<TXOutput> m_out;
};


class CoinBaseTransaction: public Transaction {
public:
    CoinBaseTransaction(uint64_t& id, std::string& address);

private:
    std::string m_address;
};

class RealTransaction: public Transaction {
public:
    RealTransaction(
        const uint64_t& id,
        const std::string& from,
        const std::string& to,
        int value,
        std::list<TXInput>& inputs,
        int rest);

private:
    std::string m_from;

    std::string m_to;

    int m_value;

    int m_rest;
};

#endif /* Transaction_hpp */
