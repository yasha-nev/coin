#include "privatekey.hpp"

PrivateKey::PrivateKey(long _d, long _n) {
    d = _d;
    n = _n;
}

PrivateKey::PrivateKey(const PrivateKey& out) {
    d = out.d;
    n = out.n;
}

PrivateKey& PrivateKey::operator=(const PrivateKey& out) {
    if(this == &out) {
        return *this;
    }

    d = out.d;
    n = out.n;

    return *this;
}

PrivateKey::PrivateKey(const std::string& path) {
    std::fstream strm;

    strm.open(path, std::ios_base::in);

    std::string key;

    strm >> key;

    std::string res = DecodeBase58(key);

    size_t found = res.find(" ");

    n = std::stoul(res.substr(0, found), nullptr, 16);
    d = std::stoul(res.substr(found), nullptr, 16);

    strm.close();
}

void PrivateKey::save(const std::string& path) {
    std::fstream strm;

    std::string hashKey = EncodeBase58(getKey());

    strm.open(path, std::ios_base::out);

    strm << hashKey;

    strm.close();
}

long PrivateKey::decodeNum(long c) {
    long result = 1;

    for(long i = 0; i < d; i++) {
        result = result * c;
        result = fmod(result, n);
    }

    return result;
}

char* PrivateKey::decrypt(long* enc, size_t size) {
    char* decode = new char[size];

    for(size_t i = 0; i < size; i++) {
        decode[i] = decodeNum(enc[i]);
    }

    return decode;
}

std::string PrivateKey::getKey() {
    std::stringstream s;
    s << std::hex << n << " " << d;
    return s.str();
}
