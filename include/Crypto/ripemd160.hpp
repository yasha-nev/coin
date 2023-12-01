#ifndef ripemd160_hpp
#define ripemd160_hpp

#include <inttypes.h>
#include <string>
#include <iostream>
#include <bit>
#include <iomanip>
#include <sstream>

using namespace std;

class RIPMD160{
public:
    RIPMD160();
    ~RIPMD160();
    uint32_t *Hash(string msg);
    string getHash();
private:
    uint32_t **CreateBlocks(string msg, uint64_t *block_count);
    void HashBlock(uint32_t *block, uint32_t *hash);
    uint32_t h0, h1, h2, h3, h4;
    uint32_t *hash;

};

#endif /* ripemd160_hpp */
