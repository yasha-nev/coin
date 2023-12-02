#include "sha256.hpp"

static uint32_t rightRotate(uint32_t n, uint32_t d)
{
    return (n >> d) | (n << (32 - d));
}

static uint32_t s0(uint32_t word){
    return rightRotate(word, 7) ^ rightRotate(word, 18) ^ (word >> 3);
}

static uint32_t s1(uint32_t word){
    return rightRotate(word, 17) ^ rightRotate(word, 19) ^ (word >> 10);
}

static uint32_t c0(uint32_t word){
    return rightRotate(word, 2) ^ rightRotate(word, 13) ^ rightRotate(word, 22);
}

static uint32_t c1(uint32_t word){
    return rightRotate(word, 6) ^ rightRotate(word, 11) ^ rightRotate(word, 25);
}

static uint32_t ch(uint32_t e, uint32_t f, uint32_t g){
    return (e & f) ^ ((~e) & g);
}

static uint32_t maj(uint32_t a, uint32_t b, uint32_t c){
    return (a & b) ^ (a & c) ^ (b & c);
}

void u32To2digit(uint32_t value){
    long k = 2147483648;
    while(k>0) {
        printf("%d", bool(value&k) );
        k=k>>1;
    };
}

sha256::sha256(){
    h0 = 0x6a09e667;
    h1 = 0xbb67ae85;
    h2 = 0x3c6ef372;
    h3 = 0xa54ff53a;
    h4 = 0x510e527f;
    h5 = 0x9b05688c;
    h6 = 0x1f83d9ab;
    h7 = 0x5be0cd19;
}

uint32_t **sha256::CreateBlocks(const std::string &msg, uint64_t *blockCount){
    uint32_t datalen = static_cast<uint32_t>(msg.size());
    uint32_t bitstr = datalen * 8;
    uint64_t payload = datalen + 8;
    *blockCount = payload / 64 + 1;
    
    uint32_t **blocks = new uint32_t *[*blockCount];
    
    uint32_t t = 0;
    
    for (uint32_t b = 0; b < *blockCount; b++){
        uint32_t *block = new uint32_t[16];
        for (int i = 0; i < 16; i++){
            block[i] = 0;
            for (int j = 0; j < 4; j++){
                if (t < datalen){
                    block[i] = msg[t] << (8 * (3 - j)) | block[i];
                    t += 1;
                }
                else if(t == datalen){
                    block[i] = (1 << 7 << (8 * (3 - j))) | block[i];
                    t += 1;
                }
                else{
                    break;
                }
            }
        }
        blocks[b] = block;
    }
    blocks[*blockCount - 1][15] = bitstr;
    
    
    return blocks;
}

void sha256::HashBlock(uint32_t *block, uint32_t *H){
    std::array<uint32_t, 64> k = {
        0x428a2f98, 0x71374491, 0xb5c0fbcf, 0xe9b5dba5, 0x3956c25b, 0x59f111f1, 0x923f82a4, 0xab1c5ed5,
        0xd807aa98, 0x12835b01, 0x243185be, 0x550c7dc3, 0x72be5d74, 0x80deb1fe, 0x9bdc06a7, 0xc19bf174,
        0xe49b69c1, 0xefbe4786, 0x0fc19dc6, 0x240ca1cc, 0x2de92c6f, 0x4a7484aa, 0x5cb0a9dc, 0x76f988da,
        0x983e5152, 0xa831c66d, 0xb00327c8, 0xbf597fc7, 0xc6e00bf3, 0xd5a79147, 0x06ca6351, 0x14292967,
        0x27b70a85, 0x2e1b2138, 0x4d2c6dfc, 0x53380d13, 0x650a7354, 0x766a0abb, 0x81c2c92e, 0x92722c85,
        0xa2bfe8a1, 0xa81a664b, 0xc24b8b70, 0xc76c51a3, 0xd192e819, 0xd6990624, 0xf40e3585, 0x106aa070,
        0x19a4c116, 0x1e376c08, 0x2748774c, 0x34b0bcb5, 0x391c0cb3, 0x4ed8aa4a, 0x5b9cca4f, 0x682e6ff3,
        0x748f82ee, 0x78a5636f, 0x84c87814, 0x8cc70208, 0x90befffa, 0xa4506ceb, 0xbef9a3f7, 0xc67178f2
    };
    
    std::array<uint32_t, 64> w;
    for (int i = 0; i < 16; i++){
        w[i] = block[i];
    }
    
    for (int i = 16; i < 64; i++){
        w[i] = w[i-16] + s0(w[i-15]) + w[i-7] + s1(w[i-2]);
    }
    
    uint32_t a = H[0];
    uint32_t b = H[1];
    uint32_t c = H[2];
    uint32_t d = H[3];
    uint32_t e = H[4];
    uint32_t f = H[5];
    uint32_t g = H[6];
    uint32_t h = H[7];
    
    for (int i = 0; i < 64; i++){
        uint32_t temp1 = h + c1(e) + ch(e, f, g) + k[i] + w[i];
        uint32_t temp2 = c0(a) + maj(a, b, c);
        
        h = g;
        g = f;
        f = e;
        e = d + temp1;
        d = c;
        c = b;
        b = a;
        a = temp1 + temp2;
    }
    
    H[0] += a;
    H[1] += b;
    H[2] += c;
    H[3] += d;
    H[4] += e;
    H[5] += f;
    H[6] += g;
    H[7] += h;
}


std::array<uint32_t, 8> sha256::Hash(const std::string &msg){
    hash[0] = h0;
    hash[1] = h1;
    hash[2] = h2;
    hash[3] = h3;
    hash[4] = h4;
    hash[5] = h5;
    hash[6] = h6;
    hash[7] = h7;
    
    uint64_t blockCount = 0;
    uint32_t **blocks = CreateBlocks(msg, &blockCount);
    for (uint64_t i = 0; i < blockCount; i++){
        HashBlock(blocks[i], hash.data());
    }
    
    for (uint64_t i = 0; i < blockCount; i++){
        delete[] blocks[i];
    }
    delete[] blocks;
    
    return hash;
}

std::string sha256::getHash(){
    std::stringstream s;

    for(int i = 0 ; i < 8 ; i++) {
        s << std::setfill('0') << std::setw(8) << std::hex << hash[i];
    }
    return s.str();
}
