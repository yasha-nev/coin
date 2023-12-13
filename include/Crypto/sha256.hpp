#ifndef sha256_hpp
#define sha256_hpp

#include <iostream>
#include <iomanip>
#include <string>
#include <sstream>
#include <inttypes.h>
#include <ctype.h>
#include <array>

/*!
    \brief реализация SHA256
*/
class sha256{
public:
    /*!
    конструктор
    */
    sha256();

    /*!
    Процесс шифрования
    \return результат хэширования массив 512 байт
    */
    std::array<uint32_t, 8> Hash(const std::string &msg);
    
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
    \param [in] blocks - массив блоков
    \param [in] H - исходные данные
    */
    void HashBlock(uint32_t *blocks, uint32_t *H);
    
    uint32_t h0, h1, h2, h3, h4, h5, h6, h7; /*!< Исходные данные */
    
    std::array<uint32_t, 8> hash; /*!< хэш*/
};

void u32To2digit(uint32_t value);

#endif
