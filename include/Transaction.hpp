#ifndef Transaction_hpp
#define Transaction_hpp

#include <inttypes.h>
#include <string>
#include <string.h>
#include <list>

#define REWARD 50

class Transaction{
public:
    uint64_t m_id;
    
    int m_inCount;
    
    int m_outCount;
    
    class TXInput *m_in;
    
    class TXOutput *m_out;
    
    Transaction(uint64_t id, int in_cout, int out_count);
    
    Transaction(Transaction *tx);
    
    ~Transaction();
    
    std::string toString();
    
    size_t size();
    
    void encode(uint8_t *ptr);
    
    void decode(uint8_t *ptr);
};

class TXOutput{
public:
    TXOutput(){};
    
    TXOutput(const TXOutput &out);
    
    TXOutput(int value, const std::string &pubkey);
    
    TXOutput &operator =(const TXOutput &out);
    
    int m_value;
    
    std::string m_pubkey;
};

class TXInput{
public:
    TXInput(){};
    
    TXInput(const TXInput &in);
    
    TXInput(const uint64_t &transId, int outIndex, const std::string &pubkey);
    
    TXInput& operator =(const TXInput &in);
    
    uint64_t m_tranId;
    
    int m_outIndex;
    
    std::string m_pubkey;
};

//Выплата за майнинг, pubkey - адрес майнера
Transaction *coinBaseTrans(const std::string &pubkey);

Transaction *simpleTrans(const uint64_t &id, const std::string &pubkey, int value);

Transaction *realTransaction(const uint64_t &id, const std::string &from, const std::string &to, int value, std::list<TXInput> inputs, int rest);

#endif /* Transaction_hpp */
