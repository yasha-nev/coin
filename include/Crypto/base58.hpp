#ifndef base58_hpp
#define base58_hpp

#include <string>

/*!
 \brief Кодирование строки в формат base58
 \param [in] source - Исходная строка
 \return Кодированная строка
*/
std::string EncodeBase58(const std::string &source);

/*!
 \brief Декодирование строки из формата base58
 \param [in] str  - Кодированная строка
 \return Декодированная строка
*/
std::string DecodeBase58(const std::string &str);

#endif /* base58_hpp */
