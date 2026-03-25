#ifndef Mainer_hpp
#define Mainer_hpp

#include "BlockChain.hpp"
#include "Network.hpp"
#include "Transaction.hpp"
#include "Wallet.hpp"
#include "privatekey.hpp"
#include "publickey.hpp"
#include "RSACryptor.hpp"

/*!
    \brief Майнер

    Интерфейс для создания клиента создающего новые блоки за вознаграждение
*/
class Mainer: public Wallet {
public:
    /*!
     \brief Конструктор с параметрами
     \param [in] bc указатель на локальный блокчейн
     \param [in] net указатель на клиента p2p сети
    */
    Mainer(BlockChain &bc, Network &net);

    /*!
     \brief Диструкток
    */
    ~Mainer();

    /*!
     \brief Процесс хэшинга новых блоков
    */
    void process();

private:
    std::thread m_thr; /*!< Поток под process*/

    std::atomic<bool> m_run; /*!< Управление потоком */

    std::mutex m_mtx; /*!< Управления данными в потоке */
};

#endif /* Mainer_hpp */
