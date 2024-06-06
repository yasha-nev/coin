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
        auto tx = m_net->getFromMempool();
        
        if (!tx){
            std::this_thread::sleep_for(50ms);
            continue;
        }
        
        std::list<Transaction> txList;
        
        txList.push_back(tx.get());
        
        uint64_t txid = tx->m_id + 1;
        std::string address = getAddres();
        txList.push_back(CoinBaseTransaction(txid, address));
        
        m_bc->createBlock(static_cast<uint64_t>(std::time(nullptr)), txList);
        
        std::this_thread::sleep_for(50ms);
    }
}
