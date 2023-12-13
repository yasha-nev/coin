#ifndef Mainer_hpp
#define Mainer_hpp

#include "privatekey.hpp"
#include "publickey.hpp"
#include "ripemd160.hpp"
#include "sha256.hpp"
#include "base58.hpp"
#include "rsa.hpp"
#include "Network.hpp"
#include "BlockChain.hpp"
#include "Transaction.hpp"
#include "Wallet.hpp"

/*!
    \brief Кошелек

    Пользовательских интерфейс для взаимодействия
    с блокчейном и сетью.
*/
class Mainer: public Wallet{
public:
    Mainer(BlockChain *bc, Network *net);
    
    ~Mainer();
    
    void process();
    
private:
    std::thread m_thr;
    
    std::atomic<bool> m_run;
    
    std::mutex m_mtx;
};

#endif /* Mainer_hpp */
