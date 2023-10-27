#ifndef Transaction_hpp
#define Transaction_hpp

#include <inttypes.h>
#include <string>
#include <list>

#define REWARD 50

using namespace std;

class Transaction{
public:
    uint64_t m_id;
    int m_inCount;
    int m_outCount;
    struct TXInput *m_in;
    struct TXOutput *m_out;
    
    
    Transaction(uint64_t id, int in_cout, int out_count);
    Transaction(Transaction *tx);
    ~Transaction();
    string toString();
    
    size_t size();
    void encode(uint8_t *ptr);
    void decode(uint8_t *ptr);
};

class TXOutput{
public:
    TXOutput(){};
    TXOutput(const TXOutput &out);
    TXOutput(int value, string pubkey);
    TXOutput &operator =(const TXOutput &out);
    
    int m_value;
    string m_pubkey;
};

class TXInput{
public:
    TXInput(){};
    TXInput(const TXInput &in);
    TXInput(uint64_t transId, int outIndex, string pubkey);
    TXInput& operator =(const TXInput &in);
    
    uint64_t m_tranId;
    int m_outIndex;
    string m_pubkey;
};

//Выплата за майнинг, pubkey - адрес майнера
Transaction *coinBaseTrans(string pubkey);
Transaction *simpleTrans(uint64_t id, string pubkey, int value);
Transaction *realTransaction(uint64_t id, string from, string to, int value, list<TXInput> inputs, int rest);
#endif /* Transaction_hpp */
