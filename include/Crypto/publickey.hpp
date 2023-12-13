#ifndef publickey_hpp
#define publickey_hpp

#include <cmath>
#include <string>
#include <sstream>
#include <fstream>
#include "base58.hpp"
#include "sha256.hpp"
#include "ripemd160.hpp"


class PublicKey{
public:
    PublicKey(){e = 0; n = 0;};
    
    PublicKey(long _e, long _n);
    
    PublicKey(const std::string &path);
    
    PublicKey(const PublicKey &out);
    
    PublicKey &operator =(const PublicKey &out);
    
    long *encrypt(char *msg, size_t size);
    
    long checkSignature(char *msg);
    
    std::string getKey();
    
    void save(const std::string &path);
    
    std::string hash();

private:
    long e;
    
    long n;
    
    long encodeChar(int c);
};
#endif /* publickey_hpp */
