#ifndef ripemd160_hpp
#define ripemd160_hpp

#include <bit>
#include <string>
#include <iomanip>
#include <sstream>
#include <inttypes.h>

class RIPMD160{
public:
    RIPMD160();
    
    std::array<uint32_t, 5> Hash(const std::string &msg);
    
    std::string getHash();
    
private:
    uint32_t **CreateBlocks(const std::string &msg, uint64_t *block_count);
    
    void HashBlock(uint32_t *block, uint32_t *hash);
    
    uint32_t h0, h1, h2, h3, h4;
    
    std::array<uint32_t, 5> hash;

};

#endif /* ripemd160_hpp */
