#include "BlockChain.hpp"
#include "Wallet.hpp"
#include "Message.hpp"

using namespace std;

void CLI(BlockChain &bc, Wallet wallet, Network &net){
    while (true){
        string command;
        cout << "CLI>";
        cin >> command;
        
        if (command == "user"){
            cout << wallet.getAddres() << "\n";
        }
        
        if (command == "send"){
            string to;
            int value;
            
            cout << "to: ";
            cin >> to;
            cout << "value: ";
            cin >> value;
            
            bc.addBlock(wallet, to, value);
        }
    
        else if (command == "balance"){
            bc.getBalance(wallet);
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
    Wallet wallet("./");
    CLI(bc, wallet, net);

    return 0;
}


