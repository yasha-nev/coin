#ifndef TXInput_hpp
#define TXInput_hpp

#include <vector>
#include <string>
#include <inttypes.h>

#include "serialization/ByteWriter.hpp"
#include "serialization/ByteReader.hpp"
#include "serialization/Serializer.hpp"

/*!
    \brief Вход транзакции

    Хранит информацию откуда были взяты средства
    Является частью Транзакции
*/
class TXInput : public Serializer {
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
     \brief Перегрузка =
    */
    TXInput& operator=(const TXInput& in);

    /*!
     \brief Перегрузка = перемещения
    */
    TXInput& operator=(const TXInput&& in);

    bool operator== (const TXInput &in) const;

    std::vector<std::byte> encode() const override;

    void decode(const std::vector<std::byte> &data) override;

    size_t size() const noexcept;

    uint64_t getTransactionId() const noexcept;

    int getOutIndex() const noexcept;

    const std::string &getPublicKey() const noexcept;
    
    const std::string &getSignature() const noexcept;

    void setSignarure(const std::string &signature);

private:
    uint64_t m_tranId; /*!< id транзакции */

    int m_outIndex; /*!< индекс выхода в транзакции*/

    std::string m_pubkey; /*!< публичный ключ */

    std::string m_sign; /*!< цифровая подпись */
};

#endif