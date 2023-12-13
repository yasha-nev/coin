#include "Mainer.hpp"

using namespace std::literals::chrono_literals;

Mainer::Mainer(BlockChain *bc, Network *net) : Wallet(bc, net){
    m_run.store(true, std::memory_order_relaxed);
    
    m_thr = std::thread(&Mainer::process, this);
    
}

Mainer::~Mainer(){
    m_run.store(false, std::memory_order_relaxed);
    
    m_thr.join();
}


void Mainer::process(){
    while (m_run.load(std::memory_order_relaxed)){
        Transaction *tx = m_net->getFromMempool();
        
        if (!tx){
            std::this_thread::sleep_for(50ms);
            continue;
        }
        
        std::list<Transaction *> txList;
        
        txList.push_back(tx);
        
        txList.push_back(coinBaseTrans(tx->m_id + 1, getAddres()));
        
        m_bc->createBlock(std::time(nullptr), txList);
        
        std::this_thread::sleep_for(50ms);
    }
}
