#ifndef publickey_hpp
#define publickey_hpp

#include <cmath>
#include <string>
#include <sstream>
#include <fstream>
#include "base58.hpp"
#include "sha256.hpp"
#include "ripemd160.hpp"


/*!
    \brief Публичный ключ
*/
class PublicKey{
public:
    /*!
    конструктор
    */
    PublicKey(){e = 0; n = 0;};
    
    /*!
    конструктор
    \param [in] _e - простое число e
    \param [in] _n - простое число n
    */
    PublicKey(long _e, long _n);
    
    /*!
    конструктор
    \param [in] path путь к расположению ключа
    */
    PublicKey(const std::string &path);
    
    /*!
    конструктор копирования
    */
    PublicKey(const PublicKey &out);
    
    /*!
    Перегрузка оператора =
    */
    PublicKey &operator =(const PublicKey &out);
    
    /*!
    Шифрование сообщения
    \param [in] msg - исходное сообщение
    \param [in] size - размер сообщения
    \return массив байт
    */
    long *encrypt(char *msg, size_t size);
    
    /*!
    проверка подписи
    \param [in] msg - исходное сообщение
    \return результат проверки
    */
    long checkSignature(char *msg);
    
    /*!
    \return строковое представление ключа
    */
    std::string getKey();

    /*!
    сохранение ключа
    \param [in] path - путь для сохранения
    */
    void save(const std::string &path);
    
    std::string hash();

private:
    long e; /*!< простое число E */
    
    long n; /*!< простое число N*/
    
    /*!
    Кодирования символа
    \param [in] c - символа
    \return кодированный символ
    */
    long encodeChar(int c);
};
#endif /* publickey_hpp */
