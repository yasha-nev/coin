#ifndef privatekey_hpp
#define privatekey_hpp

#include <cmath>
#include <string>
#include <sstream>

using namespace std;

class PrivateKey{
public:
    PrivateKey(long _d, long _n);
    char *decrypt(long *enc, size_t size);
    long signature(char *msg);
    string getKey();

private:
    long d;
    long n;

    long decodeNum(int c);
};


#endif /* privatekey_hpp */
