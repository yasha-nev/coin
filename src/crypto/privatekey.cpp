#include "privatekey.hpp"

PrivateKey::PrivateKey(long _d, long _n){
    d = _d;
    n = _n;
}

long PrivateKey::decodeNum(int c){
    long result = 1;

    for (long i = 0; i < d; i++){
        result = result * c;
        result = fmod(result, n);
    }

    return result;
}

char* PrivateKey::decrypt(long *enc, size_t size){
    char *decode = new char[size];

    for (size_t i = 0; i < size; i++){
        decode[i] = decodeNum(enc[i]);
    }

    return decode;
}

string PrivateKey::getKey(){
    std::stringstream s;
    s << std::hex << n << d;
    return s.str();
}
