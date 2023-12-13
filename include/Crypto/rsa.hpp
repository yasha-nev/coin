#ifndef rsa_hpp
#define rsa_hpp

#include "privatekey.hpp"
#include "publickey.hpp"

/*!
    \brief реализация ключей RSA
*/
class RSACryptor{
public:
    /*!
    конструктор
    */
    RSACryptor();
    
    /*!
    деструктор
    */
    ~RSACryptor();

    /*!
    \return созданный приватный ключ
    */
    PrivateKey * getPrivateKey();
    
    /*!
    \return созданный публичный ключ
    */
    PublicKey * getPublicKey();

    /*!
    Шифрование сообщения
    \param [in] msg - исходное сообщение
    \param [in] len - длина сообщения
    \return массив байт
    */
    long* enc (char* msg, size_t len);
    
    /*!
    расшифровка сообщения
    \param [in] enc - массив байт
    \param [in] len - размер сообщения
    \return расшифрованное сообщение
    */
    char* dec(long * enc, size_t len);

private:
    PrivateKey *privKey; /*!< приватный ключ*/
    
    PublicKey *pubKey; /*!< публичный ключ*/
    
    /*!
    Создание ключей
    */
    void createKeys();
};

#endif /* rsa_hpp */
