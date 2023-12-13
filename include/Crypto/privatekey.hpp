#ifndef privatekey_hpp
#define privatekey_hpp

#include <cmath>
#include <string>
#include <sstream>
#include <fstream>
#include "base58.hpp"

class PrivateKey{
public:
    PrivateKey(){d = 0; n = 0;};
    
    PrivateKey(const std::string &path);
    
    PrivateKey(long _d, long _n);
    
    PrivateKey(const PrivateKey &out);
    
    PrivateKey &operator =(const PrivateKey &out);
    
    char *decrypt(long *enc, size_t size);
    
    long signature(char *msg);
    
    std::string getKey();
    
    void save(const std::string &path);

private:
    long d;
    
    long n;

    long decodeNum(long c);
};


#endif /* privatekey_hpp */
