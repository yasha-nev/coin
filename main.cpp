#include <iostream>
#include "BlockChain.hpp"
#include "Wallet.hpp"
#include "Message.hpp"

using namespace std;

void CLI(BlockChain &bc, std::array<Wallet, 2> &wallets, Network &net){
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
        
        else if(command == "updatedb"){
            net.getBlocks();
        }
        
        else if(command == "connect"){
            string host;
            int port;
            cout << "host: ";
            cin >> host;
            cout << "port: ";
            cin >> port;
            
            net.connectTo(host, port);
        }
        
        else{
            continue;
        }
    }
}

int main(int argc, const char * argv[]) {
    if (argc < 2){
        return 0;
    }
    int port = stoi(argv[1]);
    
    BlockChain bc;
    Network net(std::list<std::pair<std::string, int>>(), port, &bc);
    std::array<Wallet, 2> wallets;
    
    // wallets[0] = Wallet("/Users/yasha_nev/projects/BlockChain/BlockChain");
    
    CLI(bc, wallets, net);

    return 0;
}


