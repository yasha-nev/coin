//
//  Wallet.hpp
//  BlockChain
//
//  Created by Яков Невидимов on 30.11.2023.
//

#ifndef Wallet_hpp
#define Wallet_hpp

#include "privatekey.hpp"
#include "publickey.hpp"
#include "ripemd160.hpp"
#include "sha256.hpp"

using namespace std;

class wallet{
public:
    string getAddres();
private:
    PrivateKey privkey;
    PublicKey pubkey;
};

#endif /* Wallet_hpp */
