#ifndef base58_hpp
#define base58_hpp

#include <string>

/*!
Кодирование строки в формат base58
\param [in] source - исходная строка
\return кодированная строка
*/
std::string EncodeBase58(const std::string &source);

/*!
Декодирование строки из формата base58
\param [in] str  - кодированная строка
\return декодированная строка
*/
std::string DecodeBase58(const std::string &str);

#endif /* base58_hpp */
