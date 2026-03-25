#ifndef rsa_hpp
#define rsa_hpp

#include <ctime>
#include "privatekey.hpp"
#include "publickey.hpp"

/*!
    \brief реализация ключей RSA
*/
class RSACryptor {
public:
    RSACryptor();

    /*!
     \brief Деструктор
    */
    ~RSACryptor();

    /*!
     \brief Получить указатель на приватный ключю
     \return Класс ключа
    */
    PrivateKey* getPrivateKey();

    /*!
     \brief Получить указатель на  публичного ключа
     \return Класс ключ
    */
    PublicKey* getPublicKey();

    /*!
     \brief Шифрование сообщения
     \param [in] msg - исходное сообщение
     \param [in] len - длина сообщения
     \return массив байт
    */
    long* enc(char* msg, size_t len);

    /*!
     \brief Расшифровка сообщения
     \param [in] enc - массив байт
     \param [in] len - размер сообщения
     \return расшифрованное сообщение
    */
    char* dec(long* enc, size_t len);

private:
    PrivateKey* privKey; /*!< приватный ключ*/

    PublicKey* pubKey; /*!< публичный ключ*/

    /*!
     \brief Создание ключей
    */
    void createKeys();
};

#endif /* rsa_hpp */
