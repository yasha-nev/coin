#ifndef Transaction_hpp
#define Transaction_hpp

#include "TXInput.hpp"
#include "TXOutput.hpp"
#include "crypto/CryptoppImpl.hpp"
#include "serialization/ByteReader.hpp"
#include "serialization/ByteWriter.hpp"
#include "serialization/Serializer.hpp"

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
class Transaction: public Serializer {
public:
    Transaction();

    /*!
    \brief Конструктор с параметрами
    \param [in] id - id транзакции
    \param [in] in_cout - количество входов
    \param [in] out_count - количество выходов
    */
    Transaction(uint64_t id, int in_cout, int out_count);

    /*!
    \brief Диструктор
    */
    ~Transaction();

    uint64_t getId() const noexcept;

    const std::vector<TXInput>& getInputs() const noexcept;

    const std::vector<TXOutput>& getOutputs() const noexcept;

    /*!
    \brief Парсит поля транзакции в строку
    */
    std::string toString() const;

    /*!
    \brief Размер транзакции в байтах
    */
    size_t size() const noexcept;

    /*!
    \brief Сериализация транзакции
    \return - массив байт для записи
    */
    void encode(ByteWriter& writer) const override;

    /*!
    \brief Десериализация транзакции
    \param [in] data - массив байт для чтения
    */
    void decode(ByteReader& reader) override;

    /*!
    \brief Подписать транзакцию
    */
    void sign();

    /*!
    \brief Вывод информации о транзакции в консоль
    */
    void print() const noexcept;

    /*!
    \brief Перегрузка оператора сравнения
    */
    bool operator==(const Transaction& tx) const;

protected:
    uint64_t m_id; /*!< идентификатор транзакции */

    std::vector<TXInput> m_in; /*!< Массив Входом */

    std::vector<TXOutput> m_out; /*!< Массив Выходов */

    friend class TransactionFactory;
};

class TransactionFactory {
public:
    static Transaction createSimple(
        const uint64_t& id,
        const std::string& from,
        const std::string& to,
        int value,
        std::list<TXInput>& inputs,
        int rest);

    static Transaction createCoinBase(const uint64_t& id, const std::string& address);
};

#endif /* Transaction_hpp */
