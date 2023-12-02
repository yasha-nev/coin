#ifndef sha256_hpp
#define sha256_hpp

#include <iostream>
#include <iomanip>
#include <string>
#include <sstream>
#include <inttypes.h>
#include <ctype.h>

class sha256{
public:
    sha256();
    
    std::array<uint32_t, 8> Hash(const std::string &msg);
    
    std::string getHash();

private:
    uint32_t **CreateBlocks(const std::string &msg, uint64_t *block_count);
    
    void HashBlock(uint32_t *blocks, uint32_t *H);
    
    uint32_t h0, h1, h2, h3, h4, h5, h6, h7;
    
    std::array<uint32_t, 8> hash;
};

void u32To2digit(uint32_t value);

#endif
