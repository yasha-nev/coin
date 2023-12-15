#include "rsa.hpp"

static bool isPrime(long prime){
    for (int i = 2; i <= sqrt(prime); i++){
        if (prime % i == 0){
            return false;
        }
    }
    return true;
}

static long generatePrime(int a){
    std::srand(static_cast<unsigned int>(std::time(nullptr)));
    long prime = a + rand() % 1000;
    while(!isPrime(prime)){
        prime++;
    }
    return prime;
}

static long evclid(long a, long b){
    while (a > 0){
        if (a < b){
            long long t = a;
            a = b;
            b = t;
        }
        a = a % b;
    }
    return b;
}

static long generateE(long t, long n){
    for (int i = 2; i < n-1; i++){
        if (evclid(t, i) == 1){
            return i;
        }
    }

    return -1;
}

static long generateD(long e, long t){
    long d;
    long k = 1;

    while ( true)
    {
        k = k + t;
        if ( k % e == 0){
            d = (k / e);
            return d;
        }
    }
}

RSACryptor::RSACryptor(){
    createKeys();
}

RSACryptor::~RSACryptor(){
    if (privKey){
        delete privKey;
    }

    if (pubKey){
        delete pubKey;
    }
}

PrivateKey * RSACryptor::getPrivateKey(){
    return privKey;
}

PublicKey * RSACryptor::getPublicKey() {
    return pubKey;
}

void RSACryptor::createKeys(){
    long phi, p, q, n, d, e;
    p = generatePrime(1024);
    q = generatePrime(1024);
    
    n = p * q;
    phi = (p - 1) * (q - 1);
    e = generateE(phi, n);
    d = generateD(e, phi);
    privKey = new PrivateKey(d, n);
    pubKey = new PublicKey(e, n);
}
