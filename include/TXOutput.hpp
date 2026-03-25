#ifndef TXOutput_hpp
#define TXOutput_hpp

#include "serialization/ByteReader.hpp"
#include "serialization/ByteWriter.hpp"
#include "serialization/Serializer.hpp"

#include <string>
#include <vector>

/*!
    \brief Выход транзакции

     Хранит информацию куда был сделал перевод
*/
class TXOutput: public Serializer {
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
     \brief Количество монет на выходе транзакции
     \return целое безнаковое число
    */
    int getValue() const noexcept;

    /*!
     \brief Адрес получателя
     \return Адрес в виде строки
    */
    const std::string& getAddress() const noexcept;

    /*!
     \brief Возвращает количество байт занимаемое выходом транзакции
     \return Количество байт
    */
    size_t size() const noexcept;

    /*!
     \brief Сериализация выхода унаследованная от Serializer
     \return выход транзакции преобразованный в массив байт
    */
    void encode(ByteWriter& writer) const override;

    /*!
     \brief Десериализация выхода унаследованная от Serializer
     \param [in] data - массив байт, сериализованный выход транзакции
    */
    void decode(ByteReader& reader) override;

    /*!
     \brief Перегрузка оператора присваивания
    */
    TXOutput& operator=(const TXOutput& out);

    /*!
     \brief Перегрузка оператора перемещения
    */
    TXOutput& operator=(const TXOutput&& out);

    /*!
     \brief Перегрузка оператора сравнения
    */
    bool operator==(const TXOutput& out) const;

private:
    int m_value; /*!< количество монет */

    std::string m_address; /*!< адрес кошелька */
};

#endif
