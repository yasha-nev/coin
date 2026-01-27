#ifndef TXOutput_hpp
#define TXOutput_hpp

#include <vector>
#include <string>

#include "serialization/ByteWriter.hpp"
#include "serialization/ByteReader.hpp"
#include "serialization/Serializer.hpp"

/*!
    \brief Выход транзакции

     Хранит информацию куда был сделал перевод
*/
class TXOutput : public Serializer {
public:
    /*!
     \brief Конструктор
    */
    TXOutput() {};

    /*!
     \brief Конструктор копирования
    */
    TXOutput(const TXOutput& out);

    /*!
     \brief Конструктор перемещения
    */
    TXOutput(const TXOutput&& out);

    /*!
     \brief Конструктор с параметрами

     \param [in] value - количество передоваемых монет
     \param [in] pubkey - адрес кошелька
    */
    TXOutput(int value, const std::string& pubkey);

    /*!
     \brief Перегрузка оператора  =
    */
    TXOutput& operator=(const TXOutput& out);

    /*!
     \brief Перегрузка оператора  = перемещения
    */
    TXOutput& operator=(const TXOutput&& out);

    bool operator == (const TXOutput &out) const;

    std::vector<std::byte> encode() const override;

    void decode(const std::vector<std::byte> &data) override;

    size_t size() const noexcept;

    int getValue() const noexcept;

    const std::string &getAddress() const noexcept;

private:

    int m_value; /*!< количество монет */

    std::string m_address; /*!< адрес кошелька */
};

#endif