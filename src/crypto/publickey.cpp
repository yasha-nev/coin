#include "publickey.hpp"

PublicKey::PublicKey(long _e, long _n){
    e = _e;
    n = _n;
}

long PublicKey::encodeChar(int c){
    long result = 1;

    for (int i = 0; i < e; i++){
        result = result * c;
        result = fmod(result , n);
    }

    return result;
}

long* PublicKey::encrypt(char *msg, size_t len){
    long *encode = new long[len];

    for (size_t i = 0; i < len; i++){
        encode[i] = encodeChar(msg[i]);
    }

    return encode;
}

string PublicKey::getKey(){
    std::stringstream s;
    s << std::hex << n << e;
    return s.str();
}
