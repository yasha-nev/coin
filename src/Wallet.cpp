#include "Wallet.hpp"

Wallet::Wallet(){
    RSACryptor rsa;
    pubkey = rsa.getPublicKey();
    privkey = rsa.getPrivateKey();
}

Wallet::Wallet(const std::string &keyPath){
    privkey = new PrivateKey(keyPath + "/priv.rem");
    pubkey = new PublicKey(keyPath + "/pub.rem");
}

void Wallet::save(const std::string &keyPath){
    privkey->save(keyPath + "/priv.rem");
    pubkey->save(keyPath + "/pub.rem");
}

std::string Wallet:: getAddres(){
    std::string pubKeyHash = publicKeyHash();
    std::string sum = checkSum(pubKeyHash);
    
    std::string src = version() + pubKeyHash + sum;
    return EncodeBase58(src);
}

std::string Wallet::checkSum(const std::string &payload){
    std::string result;
    return result;
}

std::string Wallet::version(){
    return "0";
}

std::string Wallet::publicKeyHash(){
    sha256 sha;
    RIPMD160 rip;
    sha.Hash(pubkey->getKey());
    rip.Hash(sha.getHash());
    return rip.getHash();
}
