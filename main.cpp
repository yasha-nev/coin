#include <iostream>
#include "BlockChain.hpp"
#include "ripemd160.hpp"
#include "base58.hpp"
#include "rsa.hpp"

using namespace std;

void CLI(BlockChain *bc){
    while (true){
        string command;
        cout << "CLI>";
        cin >> command;
        
        if (command == "send"){
            string from;
            string to;
            int value;
            
            cout << "from: ";
            cin >> from;
            
            cout << "to: ";
            cin >> to;
            cout << "value: ";
            cin >> value;
            
            bc->addBlock(from, to, value);
        }
    
        else if (command == "balance"){
            string user;
            
            cout << "user: ";
            cin >> user;
            bc->getBalance(user);
        }
        
        else if (command == "print"){
            bc->printChain();
        }
        
        else if (command == "quit"){
            break;
        }
        
        else{
            continue;
        }
    }
}

static int bigIntCmp(uint32_t *left, uint32_t *right){
    for (int i = 0; i < 8; i++){
        if (left[i] > right[i]){
            return 1;
        }
        else if (left[i] < right[i]){
            return -1;
        }
    }
    return 0;
}

int main(int argc, const char * argv[]) {
    BlockChain bc;
    CLI(&bc);
    
    return 0;
}

/*
RSACryptor rsa;
RIPMD160 rip;
sha256   sha;

string pubkey = rsa.getPublicKey()->getKey();
sha.Hash(pubkey);
rip.Hash(sha.getHash());
string publicKeyHash = rip.getHash();
sha.Hash(publicKeyHash);
string checkSum = sha.getHash();
string src = "0x0" + publicKeyHash + checkSum;

char *res =  new char[src.size() + 50];

EncodeBase58(const_cast<char*>(src.c_str()), static_cast<int>(src.size()), res, static_cast<int>(src.size() + 50));

cout << src << "\n" << endl;
cout << res << " " << endl;
*/
