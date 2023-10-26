#ifndef Transaction_hpp
#define Transaction_hpp

#include <inttypes.h>
#include <string>

#define REWARD 50

using namespace std;

class Transaction{
public:
    uint64_t id;
    int in_count;
    int out_count;
    struct TXInput *in;
    struct TXOutput *out;
    
    
    Transaction(uint64_t id, int in_cout, int out_count);
    Transaction(Transaction *tx);
    ~Transaction();
    string toString();
    
    size_t size();
    void encode(uint8_t *ptr);
    void decode(uint8_t *ptr);
};

struct TXOutput{
    int value;
    string pubkey;
};

struct TXInput{
    uint64_t tranId;
    int outIndex;
    string pubkey;
};

//Выплата за майнинг, pubkey - адрес майнера
Transaction *coinBaseTrans(string pubkey);
//void freeTransaction(Transaction **tx);
//string transactionToString(Transaction *tx);
//void cpyTransaction(Transaction *tx_left, Transaction *tx_right);
#endif /* Transaction_hpp */
