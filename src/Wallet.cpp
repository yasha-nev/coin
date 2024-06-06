#include "Wallet.hpp"

Wallet::Wallet(BlockChain *bc, Network *net){
    m_bc = bc;
    m_net = net;
    
    if (!file_exist("./priv.rem") || !file_exist("./pub.rem")){
        RSACryptor rsa;
        m_pubkey = PublicKey(*rsa.getPublicKey());
        m_privkey = PrivateKey(*rsa.getPrivateKey());
    }
    else{
        m_privkey = PrivateKey("./priv.rem");
        m_pubkey = PublicKey("./pub.rem");
    }
}

Wallet::~Wallet(){
    m_privkey.save("./priv.rem");
    m_pubkey.save("./pub.rem");
}

std::string Wallet::getAddres(){
    std::string pubKeyHash = m_pubkey.hash();
    std::string sum = checkSum(pubKeyHash);
    
    std::string src = version() + pubKeyHash + sum;
    
    std::string res = EncodeBase58(src);
    
    res.pop_back();
    
    return res;
}

uint64_t Wallet::getBalance(){
    return m_bc->getBalance(m_pubkey.getKey(), getAddres());
}

std::string Wallet::checkSum(const std::string &payload){
    std::string result;
    return result;
}

std::string Wallet::version(){
    return "0";
}

void Wallet::createTransaction(const std::string &address, int value){
    int rest;
    
    std::list<TXInput> inputs = m_bc->getInputs(m_pubkey.getKey(), getAddres(), value, &rest);
    if (inputs.empty()){
        std::cout << "not money" << std::endl;
        return;
    }
    Transaction *tx = new RealTransaction(m_bc->getPastTransactionId() + 1, getAddres(), address, value, inputs, rest);
    
    transactionSign(tx);
    
    m_net->sendToMempool(tx);
    
    delete tx;
}

void Wallet::transactionSign(Transaction *tx){
    std::string signstr;
    for (size_t i = 0; i < tx->m_out.size(); i++){
        signstr += tx->m_out[i].m_address;
    }

    for (size_t i = 0; i < tx->m_in.size(); i++){
        sha256 cryptor;
        std::string key = tx->m_in[i].m_pubkey + signstr;
        cryptor.Hash(key);
        tx->m_in[i].m_sign = cryptor.getHash();
    }
    return;
}
