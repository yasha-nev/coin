#ifndef publickey_hpp
#define publickey_hpp

#include <cmath>
#include <string>
#include <sstream>

using namespace std;

class PublicKey{
public:
    PublicKey(long _e, long _n);
    long *encrypt(char *msg, size_t size);
    long checkSignature(char *msg);
    string getKey();

private:
    long e;
    long n;
    long encodeChar(int c);
};
#endif /* publickey_hpp */
