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
    конструктор
    */
    RIPMD160();

    /*!
    Процесс шифрования
    \return результат хэширования массив 512 байт
    */
    std::array<uint32_t, 5> Hash(const std::string &msg);

    /*!
    \return хэш в виде строки
    */
    std::string getHash();

private:
    /*!
    Деление сообщения на блоки
    \param [in] msg - исходное сообщение
    \param [out] block_count - количество блоков
    \return массив блоков
    */
    uint32_t **CreateBlocks(const std::string &msg, uint64_t *block_count);

    /*!
    процесс хэширования блока
    \param [in] block  - массив блоков
    \param [in] hash - исходные данные
    */
    void HashBlock(uint32_t *block, uint32_t *hash);

    uint32_t h0, h1, h2, h3, h4; /*!< Исходные данные */

    std::array<uint32_t, 5> hash;  /*!< хэш*/
};

#endif /* ripemd160_hpp */
