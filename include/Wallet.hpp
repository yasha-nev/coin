#ifndef Wallet_hpp
#define Wallet_hpp

#include "privatekey.hpp"
#include "publickey.hpp"
#include "ripemd160.hpp"
#include "sha256.hpp"
#include "base58.hpp"
#include "rsa.hpp"

class Wallet{
public:
    Wallet();
    
    Wallet(const std::string &keyPath);
    
    void save(const std::string &keyPath);
    
    std::string getAddres();
    
private:
    std::string checkSum(const std::string &payload);
    
    std::string version();
    
    std::string publicKeyHash();
    
    PrivateKey *privkey;
    
    PublicKey *pubkey;
};

#endif /* Wallet_hpp */
