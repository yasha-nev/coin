#ifndef rsa_hpp
#define rsa_hpp

#include "privatekey.hpp"
#include "publickey.hpp"

class RSACryptor{
public:
    RSACryptor();
    ~RSACryptor();

    PrivateKey * getPrivateKey();
    PublicKey * getPublicKey();

    long* enc (char* msg, size_t len);
    char* dec(long * enc, size_t len);

private:
    PrivateKey *privKey;
    PublicKey *pubKey;
    void createKeys();
};

#endif /* rsa_hpp */
