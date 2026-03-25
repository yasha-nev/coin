#ifndef publickey_hpp
#define publickey_hpp

#include "base58.hpp"
#include "CryptoppImpl.hpp"

#include <cmath>
#include <fstream>
#include <sstream>
#include <string>

/*!
    \brief Публичный ключ
*/
class PublicKey {
public:
    /*!
     \brief Конструктор
    */
    PublicKey() {
        e = 0;
        n = 0;
    };

    /*!
     \brief Конструктор
     \param [in] _e - простое число e
     \param [in] _n - простое число n
    */
    PublicKey(long _e, long _n);

    /*!
     \brief Конструктор
     \param [in] path путь к расположению ключа
    */
    PublicKey(const std::string& path);

    /*!
     \brief Конструктор копирования
    */
    PublicKey(const PublicKey& out);

    /*!
     \brief Перегрузка оператора =
    */
    PublicKey& operator=(const PublicKey& out);

    /*!
     \brief Шифрование сообщения
     \param [in] msg - исходное сообщение
     \param [in] size - размер сообщения
     \return массив байт
    */
    long* encrypt(char* msg, size_t size);

    /*!
     \brief Проверка подписи
     \param [in] msg - исходное сообщение
     \return результат проверки
    */
    long checkSignature(char* msg);

    /*!
     \brief Представления ключа в hex формате
     \return Строковое представление ключа
    */
    std::string getKey();

    /*!
     \brief Сохранение ключа
     \param [in] path - путь для сохранения
    */
    void save(const std::string& path);

    /*!
     \brief перевод ключа в формат base58
     \return хэш строки
    */
    std::string hash();

private:
    long e; /*!< простое число E */

    long n; /*!< простое число N*/

    /*!
     \brief Кодирования символа
     \param [in] c - символа
     \return кодированный символ
    */
    long encodeChar(int c);
};
#endif /* publickey_hpp */
