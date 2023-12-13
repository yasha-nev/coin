#ifndef privatekey_hpp
#define privatekey_hpp

#include <cmath>
#include <string>
#include <sstream>
#include <fstream>
#include "base58.hpp"

/*!
    \brief Приватный ключ
*/
class PrivateKey{
public:
    /*!
    конструктор
    */
    PrivateKey(){d = 0; n = 0;};
    
    /*!
    конструктор
    \param [in] path путь к расположению ключа
    */
    PrivateKey(const std::string &path);
    
    /*!
    конструктор
    \param [in] _d -  простое число D
    \param [in] _n - простое число N
    */
    PrivateKey(long _d, long _n);
    
    /*!
    конструктор копирования
    */
    PrivateKey(const PrivateKey &out);
    
    /*!
         Перегрузка оператора =
    */
    PrivateKey &operator =(const PrivateKey &out);
    
    /*!
    Расшифрование сообщения
    \param [in] enc - массив байт
    \param [in] size - размер сообщения
    \return исходное сообщение
    */
    char *decrypt(long *enc, size_t size);

    /*!
    Создание подписи
    \param [in] msg - исходное сообщение
    \return результат подписи
    */
    long signature(char *msg);
    
    /*!
    \return строковое представление ключа
    */
    std::string getKey();
    
    /*!
    сохранение ключа
    \param [in] path - путь для сохранения
    */
    void save(const std::string &path);

private:
    long d; /*!< простое число D */
    
    long n; /*!< простое число N */

    /*!
    Декодирование символа
    \param [in] c - кодированный символ
    \return декодированный символ
    */
    long decodeNum(long c);
};


#endif /* privatekey_hpp */
