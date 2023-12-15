#ifndef ripemd160_hpp
#define ripemd160_hpp

#include <bit>
#include <string>
#include <iomanip>
#include <sstream>
#include <inttypes.h>
#include <array>

/*!
    \brief реализация алгоритма RIPMD160
*/

class RIPMD160{
public:
    /*!
     \brief Конструктор
    */
    RIPMD160();

    /*!
     \brief Процесс шифрования
     \return Результат хэширования массив 512 байт
    */
    std::array<uint32_t, 5> Hash(const std::string &msg);

    /*!
     \brief Результат хэширования в hex формате
     \return хэш
    */
    std::string getHash();

private:
    /*!
     \brief Деление сообщения на блоки
     \param [in] msg - исходное сообщение
     \param [out] block_count - количество блоков
     \return массив блоков
    */
    uint32_t **CreateBlocks(const std::string &msg, uint64_t *block_count);

    /*!
     \brief Процесс хэширования блока
     \param [in] block  - массив блоков
     \param [in] hash - исходные данные
    */
    void HashBlock(uint32_t *block, uint32_t *hash);

    uint32_t h0, h1, h2, h3, h4; /*!< Исходные данные */

    std::array<uint32_t, 5> hash;  /*!< хэш */
};

#endif /* ripemd160_hpp */
