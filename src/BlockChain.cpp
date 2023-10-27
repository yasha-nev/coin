#include "BlockChain.hpp"

static inline bool file_exist (const std::string& name) {
    if (FILE *file = fopen(name.c_str(), "r")) {
        fclose(file);
        return true;
    } else {
        return false;
    }
}

static bool checkFirstBlock(const uint32_t *hash){
    bool flag = true;
    
    for (int i = 0; i < 8; i++){
        if (hash[i] != 0) {
            flag = false;
        }
    }
    
    return flag;
}

BlockChain::BlockChain(){
    if (file_exist(DBPATH)){
        m_db.connect();
        string current_hash = m_db.getCurrentHash();
        memcpy(cur_hash, current_hash.c_str(), sizeof(uint32_t) * 8);
        blocksIDs = m_db.getCurrentId((uint32_t *) current_hash.c_str());
    }
    else{
        m_db.connectIfexist();
        Block *block = genesisBlock();
        memcpy(cur_hash, block->getHash(), sizeof(uint32_t) * 8);
        m_db.putBlock(block);
        blocksIDs = 0;
        delete block;
    }
}

BlockChain::~BlockChain(){
}

void BlockChain::addBlock(string from, string to, int value){
    blocksIDs++;
    int rest;
    list<TXInput> inputs = createInputs(from, value, &rest);
    if (inputs.empty()){
        cout << "not money" << endl;
        return;
    }
    Transaction *tx = realTransaction(blocksIDs, from, to, value, inputs, rest);
    Block *block_n = newBlock(tx, cur_hash);
    
    m_db.putBlock(block_n);
    memcpy(cur_hash, block_n->getHash(), sizeof(uint32_t) * 8);
    delete block_n;
    delete tx;
}

Block* BlockChain::genesisBlock(){
    uint32_t zero_hash[8] = {0, 0, 0, 0, 0, 0, 0, 0};
    Transaction * coinBase = coinBaseTrans("yasha");
    Block *block_n = newBlock(coinBase, zero_hash);
    array2String(block_n->getHash());
    memcpy(cur_hash, block_n->getHash(), sizeof(uint32_t) * 8);
    delete coinBase;
    return block_n;
}

void BlockChain::printChain(){
    uint32_t hash[8];
    
    memcpy(hash, cur_hash, sizeof(uint32_t) * 8);
    string blc;
    
    while (!checkFirstBlock(hash)){
        Block *block = getBlock((uint32_t *) hash);
        memcpy(hash, block->getPrevBlockHash(), sizeof(uint32_t) * 8);
        block->print();
        delete block;
    }
}

Block *BlockChain::newBlock(Transaction *tx, uint32_t *prevHashBlock){
    Block *block_n = new Block( static_cast<uint64_t>(std::time(nullptr)),
                               tx,
                               prevHashBlock,
                               new uint32_t[8],
                               0);
    
    ProofOfWork pow(block_n);
    pow.Run();
    return block_n;
}

Block *BlockChain::getBlock(uint32_t *hash){
    return m_db.getBlockByHash(hash);
}

TXOutput *getOutputs(string from, int value, int *count){
    return nullptr;
}

uint64_t BlockChain::getBalance(string user){
    uint64_t sum = 0;
    uint32_t hash[8];
    
    struct outIds{
        uint64_t id;
        int outIndex;
    };
    
    struct outIds outIds[blocksIDs];
    size_t it = 0;
    
    memcpy(hash, cur_hash, sizeof(uint32_t) * 8);
    string blc;
    
    while (!checkFirstBlock(hash)){
        Block *block = getBlock((uint32_t *) hash);
        memcpy(hash, block->getPrevBlockHash(), sizeof(uint32_t) * 8);
        
        Transaction *tx = block->getTransaction();
        
        int l = 0;
        for (; l < it; l++){
            if (tx->m_id == outIds[l].id){
                break;
            }
        }
        
        for (int i = 0; i < tx->m_outCount; i++){
            if (tx->m_out[i].m_pubkey == user){
                if (l < it && outIds[l].outIndex == i){
                    break;
                }
                
                sum += tx->m_out[i].m_value;
            }
        }
        
        for (int i = 0; i < tx->m_inCount; i++){
            if (tx->m_in[i].m_pubkey == user){
                outIds[it] = {tx->m_in[i].m_tranId, tx->m_in[i].m_outIndex};
                it++;
            }
        }
        
        delete block;
    }
    cout << "=================Balance=================\n";
    cout << "user: " << sum << "\n" << endl;
    
    return sum;
}

list<TXInput> BlockChain::createInputs(string from, int value, int *rest){
    list<TXInput> ls;
    int sum = 0;
    uint32_t hash[8];
    *rest = 0;
    
    
    struct outIds{
        uint64_t id;
        int outIndex;
    };
    
    struct outIds outIds[blocksIDs];
    size_t it = 0;
    
    memcpy(hash, cur_hash, sizeof(uint32_t) * 8);
    string blc;
    
    while (!checkFirstBlock(hash)){
        Block *block = getBlock((uint32_t *) hash);
        memcpy(hash, block->getPrevBlockHash(), sizeof(uint32_t) * 8);
        
        Transaction *tx = block->getTransaction();
        
        int l = 0;
        for (; l < it; l++){
            if (tx->m_id == outIds[l].id){
                break;
            }
        }
        
        for (int i = 0; i < tx->m_outCount; i++){
            if (tx->m_out[i].m_pubkey == from){
                if (l < it && outIds[l].outIndex == i){
                    break;
                }
                ls.push_back(TXInput(tx->m_id, i, from));
                sum += tx->m_out[i].m_value;
            }
        }
        
        for (int i = 0; i < tx->m_inCount; i++){
            if (tx->m_in[i].m_pubkey == from){
                outIds[it] = {tx->m_in[i].m_tranId, tx->m_in[i].m_outIndex};
                it++;
            }
        }
        
        if (sum >= value){
            break;
        }
        
        delete block;
    }
    
    if (sum < value){
        ls.clear();
    }
    *rest = sum - value;
    
    return ls;
}
