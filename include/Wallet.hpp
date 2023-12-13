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

class Wallet{
public:
    Wallet(BlockChain *bc, Network *net);
    
    ~Wallet();
    
    std::string getAddres();
    
    uint64_t getBalance();
    
    void createTransaction(const std::string &address, int value);
    
protected:
    
    void transactionSign(Transaction *tx);
    
    std::string checkSum(const std::string &payload);
    
    std::string version();
    
    PrivateKey m_privkey;
    
    PublicKey m_pubkey;
    
    BlockChain *m_bc;
    
    Network *m_net;
};

#endif /* Wallet_hpp */
