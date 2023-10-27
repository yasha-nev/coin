#include <iostream>
#include "BlockChain.hpp"

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


int main(int argc, const char * argv[]) {
    BlockChain bc;
    CLI(&bc);
    return 0;
}
