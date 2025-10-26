#include "publickey.hpp"

PublicKey::PublicKey(long _e, long _n) {
    e = _e;
    n = _n;
}

PublicKey::PublicKey(const PublicKey& out) {
    e = out.e;
    n = out.n;
}

PublicKey& PublicKey::operator=(const PublicKey& out) {
    if(this == &out) {
        return *this;
    }

    e = out.e;
    n = out.n;

    return *this;
}

PublicKey::PublicKey(const std::string& path) {
    std::fstream strm;

    strm.open(path, std::ios_base::in);

    std::string key;

    strm >> key;

    std::string res = DecodeBase58(key);
    size_t found = res.find(" ");

    n = std::stoul(res.substr(0, found), nullptr, 16);
    e = std::stoul(res.substr(found), nullptr, 16);

    strm.close();
}

void PublicKey::save(const std::string& path) {
    std::fstream strm;

    strm.open(path, std::ios_base::out);
    strm << EncodeBase58(getKey());

    strm.close();
}

long PublicKey::encodeChar(int c) {
    long result = 1;

    for(long i = 0; i < e; i++) {
        result = result * c;
        result = fmod(result, n);
    }

    return result;
}

long* PublicKey::encrypt(char* msg, size_t len) {
    long* encode = new long[len];

    for(size_t i = 0; i < len; i++) {
        encode[i] = encodeChar(msg[i]);
    }

    return encode;
}

std::string PublicKey::getKey() {
    std::stringstream s;
    s << std::hex << n << " " << e;
    return s.str();
}

std::string PublicKey::hash() {
    sha256 sha;
    RIPMD160 rip;
    sha.Hash(getKey());
    rip.Hash(sha.getHash());
    return rip.getHash();
}
