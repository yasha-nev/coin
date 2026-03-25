#pragma once

#include "BlockChain.hpp"
#include "Transaction.hpp"
#include "crypto/ICrypto.hpp"
#include "crypto/RSACryptor.hpp"
#include "crypto/base58.hpp"
#include "crypto/privatekey.hpp"
#include "crypto/publickey.hpp"
#include "net/Network.hpp"

/*!
    \brief Кошелек

    Пользовательских интерфейс для взаимодействия
    с блокчейном и сетью.
*/
class Wallet {
public:
    /*!
     \brief Базовый конструктор
     \param [in] bc - локальная цепь блоков
     \param [in] net - p2p сеть
    */
    Wallet(BlockChain& bc, Network& net);

    /*!
     \brief Диструктор
    */
    ~Wallet();

    /*!
     \brief Генерирует адресс кошелька
     \return строка с адрессом
    */
    std::string getAddres();

    /*!
     \brief Баланс кошелька
     \return  сумма
    */
    uint64_t getBalance();

    /*!
     \brief Создать форму транзакции
     \param [in] address - адрес перевода
     \param [in] value - сумма перевода
    */
    void createTransaction(const std::string& address, int value);

protected:
    /*!
     \brief Создания хэша для проверки
     \param [in] payload ресурс хэширования
     \return строка хэша в hex формате
    */
    std::string checkSum(const std::string& payload);

    /*!
     \brief Версия кошелька
     \return строка версии
    */
    std::string version();

    PrivateKey m_privkey; /*!< приватный ключ*/

    PublicKey m_pubkey; /*!< Публичный ключ*/

    BlockChain& m_bc; /*!< Локальная цепь блоков*/

    Network& m_net; /*!<  экземпляр p2p сети*/
};
