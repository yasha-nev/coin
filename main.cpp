#include <iostream>
#include "BlockChain.hpp"
#include "Wallet.hpp"

using namespace std;

void CLI(BlockChain &bc, std::array<Wallet, 2> &wallets){
    while (true){
        string command;
        cout << "CLI>";
        cin >> command;
        
        if (command == "users"){
            for (int i = 0; i < wallets.size(); i++){
                cout << i << " " << wallets[i].getAddres() << "\n";
            }
        }
        
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
            
            bc.addBlock(from, to, value);
        }
    
        else if (command == "balance"){
            string user;
            
            cout << "user: ";
            cin >> user;
            bc.getBalance(user);
        }
        
        else if (command == "print"){
            bc.printChain();
        }
        
        else if (command == "quit"){
            break;
        }
        
        else{
            continue;
        }
    }
}

int main(int argc, const char * argv[]) {
    (void) argc;
    (void) argv;
    
    BlockChain bc;
    std::array<Wallet, 2> wallets;
    
    //wallets[0] = Wallet("/Users/yasha_nev/projects/BlockChain/BlockChain");
    
    CLI(bc, wallets);
    
    return 0;
}


