#ifndef TXInput_hpp
#define TXInput_hpp

#include "serialization/ByteReader.hpp"
#include "serialization/ByteWriter.hpp"
#include "serialization/Serializer.hpp"

#include <inttypes.h>
#include <string>
#include <vector>

/*!
    \brief Вход транзакции

    Хранит информацию откуда были взяты средства
    Является частью Транзакции
*/
class TXInput: public Serializer {
public:
    /*!
     \brief Конструктор
    */
    TXInput() {};

    /*!
     \brief Конструктор копирования
    */
    TXInput(const TXInput& in);

    /*!
     \brief Конструктор перемещения
    */
    TXInput(const TXInput&& in);

    /*!
     \brief Конструктор с параметрами
     \param [in] transId - id транзакции
     \param [in] outIndex - индекс выхода в транзакции
     \param [in] pubkey - публичный ключ отправителя
    */
    TXInput(const uint64_t& transId, int outIndex, const std::string& pubkey);

    /*!
     \brief Идентификатора транзакции
     \return идентификатор - целое безнаковое число
    */
    uint64_t getTransactionId() const noexcept;

    /*!
     \brief Индекс выходной в транзакции, которая использовалась в текущей
     \return Индексное число
    */
    int getOutIndex() const noexcept;

    /*!
     \brief Публичный ключ владельца
     \return Публичный ключ в виде строки
    */
    const std::string& getPublicKey() const noexcept;

    /*!
     \brief Подпись входа
     \return подпись в виде строки
    */
    const std::string& getSignature() const noexcept;

    /*!
     \brief Установка подписи входа транзакции
     \param [in] signature - подпись транзакии в виде строки
    */
    void setSignarure(const std::string& signature);

    /*!
     \brief Сериализация входа унаследованная от Serializer
     \return Вход транзакции преобразованный в массив байт
    */
    void encode(ByteWriter& writer) const override;

    /*!
     \brief Десериализация входа унаследованная от Serializer
     \param [in] data - массив байт, сериализованный вход транзакции
    */
    void decode(ByteReader& reader) override;

    /*!
     \brief Возвращает количество байт занимаемое входом
     \return количество байт
    */
    size_t size() const noexcept;

    /*!
     \brief Перегрузка присваивания присваивания
    */
    TXInput& operator=(const TXInput& in);

    /*!
     \brief Перегрузка оператора перемещения
    */
    TXInput& operator=(const TXInput&& in);

    /*!
     \brief Перегрузка оператора сравнеия
    */
    bool operator==(const TXInput& in) const;

private:
    uint64_t m_tranId; /*!< id транзакции */

    int m_outIndex; /*!< индекс выхода в транзакции*/

    std::string m_pubkey; /*!< публичный ключ */

    std::string m_sign; /*!< цифровая подпись */
};

#endif
