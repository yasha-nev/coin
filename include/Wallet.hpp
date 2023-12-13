#ifndef Wallet_hpp
#define Wallet_hpp

#include "privatekey.hpp"
#include "publickey.hpp"
#include "ripemd160.hpp"
#include "sha256.hpp"
#include "base58.hpp"
#include "rsa.hpp"
#include "Network.hpp"
#include "BlockChain.hpp"
#include "Transaction.hpp"

/*!
    \brief Кошелек

    Пользовательских интерфейс для взаимодействия
    с блокчейном и сетью.
*/
class Wallet{
public:
    /*!
    Базовый конструктор
     \param [in] bc - локальная цепь блоков
     \param [in] net - p2p сеть
    */
    Wallet(BlockChain *bc, Network *net);
    
    /*!
    Диструктор
    */
    ~Wallet();
    
    /*!
    Генерирует адресс кошелька
    \return строка с адрессом
    */
    std::string getAddres();
    
    /*!
    Баланс кошелька
    \return  сумма
    */
    uint64_t getBalance();
    
    /*!
     Создать форму транзакции
    \param [in] address - адрес перевода
    \param [in] value - сумма перевода
    */
    void createTransaction(const std::string &address, int value);
    
protected:
    /*!
     Подписать транзакцию
    \param [in] tx - адрес перевода
    */
    void transactionSign(Transaction *tx);
    

    std::string checkSum(const std::string &payload);
    
    /*!
    \return версия кошелька
    */
    std::string version();
    
    PrivateKey m_privkey; /*!< приватный ключ*/
    
    PublicKey m_pubkey; /*!< Публичный ключ*/
    
    BlockChain *m_bc; /*!< Локальная цепь блоков*/
    
    Network *m_net; /*!<  экземпляр p2p сети*/
};

#endif /* Wallet_hpp */
