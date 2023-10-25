#include <iostream>
#include "BlockChain.hpp"


#include <assert.h>
#include <leveldb/db.h>


using namespace std;


int main(int argc, const char * argv[]) {
    BlockChain bc;
    
    //bc.addBlock("Send 1 btc to yasha");
    
    //bc.addBlock("Send 2 btc to Oleg");
    
    bc.printChain();
    return 0;
}
