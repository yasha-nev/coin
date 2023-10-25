#ifndef sha256_hpp
#define sha256_hpp

#include <iostream>
#include <iomanip>
#include <string>
#include <inttypes.h>
#include <ctype.h>

using namespace std;

class sha256{
public:
    sha256();
    ~sha256();
    uint32_t *Hash(string msg);
private:
    uint32_t **CreateBlocks(string msg, uint64_t *block_count);
    void HashBlock(uint32_t *blocks, uint32_t *H);
    uint32_t *hash;
};

void u32To2digit(uint32_t value);

#endif
