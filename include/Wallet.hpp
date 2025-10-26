#ifndef Wallet_hpp
#define Wallet_hpp

#include "BlockChain.hpp"
#include "Network.hpp"
#include "Transaction.hpp"
#include "base58.hpp"
#include "privatekey.hpp"
#include "publickey.hpp"
#include "ripemd160.hpp"
#include "rsa.hpp"
#include "sha256.hpp"

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
    Wallet(BlockChain* bc, Network* net);

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
     \brief Подписать транзакцию
     \param [in] tx - адрес перевода
    */
    void transactionSign(std::unique_ptr<Transaction>& tx);

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

    BlockChain* m_bc; /*!< Локальная цепь блоков*/

    Network* m_net; /*!<  экземпляр p2p сети*/
};

#endif /* Wallet_hpp */
