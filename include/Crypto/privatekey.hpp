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
     \brief Конструктор
    */
    PrivateKey(){d = 0; n = 0;};
    
    /*!
     \brief Конструктор
     \param [in] path путь к расположению ключа
    */
    PrivateKey(const std::string &path);
    
    /*!
     \brief Конструктор
     \param [in] _d -  простое число D
     \param [in] _n - простое число N
    */
    PrivateKey(long _d, long _n);
    
    /*!
     \brief Конструктор копирования
    */
    PrivateKey(const PrivateKey &out);
    
    /*!
     \brief Перегрузка оператора =
    */
    PrivateKey &operator =(const PrivateKey &out);
    
    /*!
     \brief Расшифровка сообщения
    \param [in] enc - массив байт
    \param [in] size - размер сообщения
    \return исходное сообщение
    */
    char *decrypt(long *enc, size_t size);

    /*!
     \brief Создание подписи
     \param [in] msg - исходное сообщение
     \return результат подписи
    */
    long signature(char *msg);
    
    /*!
     \brief Представления клуча в hex форамате
     \return строковое представление ключа
    */
    std::string getKey();
    
    /*!
     \brief Сохранение ключа
     \param [in] path - путь для сохранения
    */
    void save(const std::string &path);

private:
    long d; /*!< простое число D */
    
    long n; /*!< простое число N */

    /*!
     \brief Декодирование символа
     \param [in] c - кодированный символ
     \return декодированный символ
    */
    long decodeNum(long c);
};


#endif /* privatekey_hpp */
