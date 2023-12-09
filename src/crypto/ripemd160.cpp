#include "ripemd160.hpp"

uint32_t rol(uint32_t a, uint32_t s){
    return ((a << s) | (a >> (32 - s)));
}

static uint32_t F(int j, uint32_t x, uint32_t y, uint32_t z){
    if (j <= 15){
        return x ^ y ^ z;
    } else if (j <= 31){
        return (x & y) | ((~x) & z);
    } else if (j <= 47){
        return (x | (~y)) ^ z;
    } else if (j <= 63){
        return (x & z) | (y & (~z));
    } else {
       return x ^ (y | (~z));
    }
}

static uint32_t K(int j){
    if      (j <= 15) return 0x00000000;
    else if (j <= 31) return 0x5A827999;
    else if (j <= 47) return 0x6ED9EBA1;
    else if (j <= 63) return 0x8F1BBCDC;
    else              return 0xA953FD4E;
}

static uint32_t K_(int j){
    if      (j <= 15) return 0x50A28BE6;
    else if (j <= 31) return 0x5C4DD124;
    else if (j <= 47) return 0x6D703EF3;
    else if (j <= 63) return 0x7A6D76E9;
    else              return 0x00000000;
}

static uint32_t R(int j){
    if (j <= 15){
        return j;
    } else if (j <= 31){
        uint32_t r[16] = {7, 4, 13, 1, 10, 6, 15, 3, 12, 0, 9, 5, 2, 14, 11, 8};
        return r[j % 16];
    } else if (j <= 47){
        uint32_t r[16] = {3, 10, 14, 4, 9, 15, 8, 1, 2, 7, 0, 6, 13, 11, 5, 12};
        return r[j % 16];
    } else if (j <= 63){
        uint32_t r[16] = {1, 9, 11, 10, 0, 8, 12, 4, 13, 3, 7, 15, 14, 5, 6, 2};
        return r[j % 16];
    } else {
        uint32_t r[16] = {4, 0, 5, 9, 7, 12, 2, 10, 14, 1, 3, 8, 11, 6, 15, 13};
        return r[j % 16];
    }
}

static uint32_t R_(int j){
    if (j <= 15){
        uint32_t r[16] = {5, 14, 7, 0, 9, 2, 11, 4, 13, 6, 15, 8, 1, 10, 3, 12};
        return r[j % 16];
        
    } else if (j <= 31){
        uint32_t r[16] = {6, 11, 3, 7, 0, 13, 5, 10, 14, 15, 8, 12, 4, 9, 1, 2};
        return r[j % 16];
        
    } else if (j <= 47){
        uint32_t r[16] = {15, 5, 1, 3, 7, 14, 6, 9, 11, 8, 12, 2, 10, 0, 4, 13};
        return r[j % 16];
        
    } else if (j <= 63){
        uint32_t r[16] = {8, 6, 4, 1, 3, 11, 15, 0, 5, 12, 2, 13, 9, 7, 10, 14};
        return r[j % 16];
        
    } else {
        uint32_t r[16] = {12, 15, 10, 4, 1, 5, 8, 7, 6, 2, 13, 14, 0, 3, 9, 11};
        return r[j % 16];
    }
}

static uint32_t S(int j){
    if (j <= 15){
        uint32_t r[16] = {11, 14, 15, 12, 5, 8, 7, 9, 11, 13, 14, 15, 6, 7, 9, 8};
        return r[j % 16];
    } else if (j <= 31){
        uint32_t r[16] = {7, 6, 8, 13, 11, 9, 7, 15, 7, 12, 15, 9, 11, 7, 13, 12};
        return r[j % 16];
    } else if (j <= 47){
        uint32_t r[16] = {11, 13, 6, 7, 14, 9, 13, 15, 14, 8, 13, 6, 5, 12, 7, 5};
        return r[j % 16];
    } else if (j <= 63){
        uint32_t r[16] = {11, 12, 14, 15, 14, 15, 9, 8, 9, 14, 5, 6, 8, 6, 5, 12};
        return r[j % 16];
    } else {
        uint32_t r[16] = {9, 15, 5, 11, 6, 8, 13, 12, 5, 12, 13, 14, 11, 8, 5, 6};
        return r[j % 16];
    }
}

static uint32_t S_(int j){
    if (j <= 15){
        uint32_t r[16] = {8, 9, 9, 11, 13, 15, 15, 5, 7, 7, 8, 11, 14, 14, 12, 6};
        return r[j % 16];
    } else if (j <= 31){
        uint32_t r[16] = {9, 13, 15, 7, 12, 8, 9, 11, 7, 7, 12, 7, 6, 15, 13, 11};
        return r[j % 16];
    } else if (j <= 47){
        uint32_t r[16] = {9, 7, 15, 11, 8, 6, 6, 14, 12, 13, 5, 14, 13, 13, 7, 5};
        return r[j % 16];
    } else if (j <= 63){
        uint32_t r[16] = {15, 5, 8, 11, 14, 14, 6, 14, 6, 9, 12, 9, 12, 5, 15, 8};
        return r[j % 16];
    } else {
        uint32_t r[16] = {8, 5, 12, 9, 12, 5, 14, 6, 8, 13, 6, 5, 15, 13, 11, 11};
        return r[j % 16];
    }
}

RIPMD160::RIPMD160(){
    h0 = 0x67452301;
    h1 = 0xEFCDAB89;
    h2 = 0x98BADCFE;
    h3 = 0x10325476;
    h4 = 0xC3D2E1F0;
}

uint32_t **RIPMD160::CreateBlocks(const std::string &msg, uint64_t *blockCount){
    uint32_t datalen = static_cast<uint32_t>(msg.size());
    uint32_t bitstr = datalen * 8;
    uint64_t payload = datalen + 8;
    *blockCount = payload / 64 + 1;
    
    uint32_t **blocks = new uint32_t *[*blockCount];
    
    uint32_t t = 0;
    
    for (uint64_t b = 0; b < *blockCount; b++){
        uint32_t *block = new uint32_t[16];
        for (int i = 0; i < 16; i++){
            block[i] = 0;
            for (int j = 0; j < 4; j++){
                if (t < datalen){
                    block[i] |= msg[t] << (j % 4 * 8 );
                    t ++;
                }
                else if(t == datalen){
                    block[i] = (1 << 7 << (j % 4 * 8 )) | block[i];
                    t ++;
                }
                else{
                    break;
                }
            }
        }
        blocks[b] = block;
    }
    blocks[*blockCount - 1][14] = bitstr;
    
    
    return blocks;
}

void RIPMD160::HashBlock(uint32_t *block, uint32_t *h){
    uint32_t T = 0;
    uint32_t A = h[0];
    uint32_t B = h[1];
    uint32_t C = h[2];
    uint32_t D = h[3];
    uint32_t E = h[4];

    uint32_t A2 = h[0];
    uint32_t B2 = h[1];
    uint32_t C2 = h[2];
    uint32_t D2 = h[3];
    uint32_t E2 = h[4];

    for (int j = 0; j < 80; j++) {
        T = std::rotl(A + F(j, B, C, D) + block[R(j)] + K(j), S(j)) + E;
        
        A = E;
        E = D;
        D = std::rotl(static_cast<uint32_t>(C), 10);
        C = B;
        B = T;
        
        T = std::rotl(A2 + F(79 - j, B2, C2, D2) + block[R_(j)] + K_(j), S_(j)) + E2;
        A2 = E2;
        E2 = D2;
        D2 = std::rotl(static_cast<uint32_t>(C2), 10);
        C2 = B2;
        B2 = T;
        
    }
    
    T = h[1] + C + D2;
    h[1] = h[2] + D + E2;
    h[2] = h[3] + E + A2;
    h[3] = h[4] + A + B2;
    h[4] = h[0] + B + C2;
    h[0] = T;
}


std::array<uint32_t, 5> RIPMD160::Hash(const std::string &msg){
    hash[0] = h0;
    hash[1] = h1;
    hash[2] = h2;
    hash[3] = h3;
    hash[4] = h4;
    
    uint64_t block_count = 0;
    uint32_t **blocks = CreateBlocks(msg, &block_count);
    for (int i = 0; i < block_count; i++){
        HashBlock(blocks[i], hash.data());
    }
    
    for (int i = 0; i < block_count; i++){
        delete[] blocks[i];
    }
    delete[] blocks;
    
    return hash;
}

std::string RIPMD160::getHash(){
    std::stringstream s;
    
    for ( uint32_t i = 0; i < 20; ++i ) {
        int8_t byte_value =  hash[i / 4] >> (i % 4 * 8);
        s << std::setfill('0') << std::setw(2) << std::hex << ( byte_value & 0xff );
    }
    
    return s.str();
}

/*
static int32_t right_shift(const uint32_t& base, const uint32_t& shift) {
    return base >> shift;
}
*/
