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
