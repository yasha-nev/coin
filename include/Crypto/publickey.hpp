#ifndef publickey_hpp
#define publickey_hpp

#include <cmath>
#include <string>
#include <sstream>
#include <fstream>
#include "base58.hpp"

class PublicKey{
public:
    PublicKey(long _e, long _n);
    
    PublicKey(const std::string &path);
    
    long *encrypt(char *msg, size_t size);
    
    long checkSignature(char *msg);
    
    std::string getKey();
    
    void save(const std::string &path);

private:
    long e;
    
    long n;
    
    long encodeChar(int c);
};
#endif /* publickey_hpp */
