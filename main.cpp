#include "BlockChain.hpp"
#include "Wallet.hpp"
#include "Message.hpp"
#include "Mainer.hpp"

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
            
            wallet.createTransaction(to, value);
        }
    
        else if (command == "balance"){
            std::cout << "=================Balance=================\n";
            std::cout << "user: " << wallet.getBalance() << "\n";
        }
        
        else if (command == "print"){
            bc.printChain();
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
        
        else if(command == "updatedb"){
            net.getBlocks();
        }
        
        else if (command == "quit"){
            break;
        }
        
        else{
            std::cout << "user      - get wallet address \n";
            std::cout << "send      - create and send Transaction \n";
            std::cout << "balance   - get wallet balance \n";
            std::cout << "connect   - connect to user of network \n";
            std::cout << "print     - print the blockchain \n";
            std::cout << "updatedb  - obtain blocks from network users \n";
            std::cout << "quit      - exit from programm \n";
            continue;
        }
    }
}

void asWallet(BlockChain &bc, Network &net){
    Wallet wallet(&bc, &net);
    CLI(bc, wallet, net);
}

void asMainer(BlockChain &bc, Network &net){
    Mainer mainer(&bc, &net);
    
    while(true){
        string command;
        cout << "CLI>";
        cin >> command;
        
        if (command == "quit"){
            return;
        }
    }
}

int main(int argc, const char * argv[]) {
    if (argc < 3){
        std::cout << "enter the port and operating mode \n";
        std::cout << "-w wallet mode, -m mainer mode\n";
        std::cout << "example ./main 5060 -w" << std::endl;
        return 0;
    }
    int port = stoi(argv[1]);
    std::string res = argv[2];
    
    
    BlockChain bc;
    Network net(std::list<std::pair<std::string, int>>(), port, &bc);
    if (res == "-w"){
        asWallet(bc, net);
    }
    else if(res=="-m"){
        asMainer(bc, net);
    }
    else{
        
    }

    return 0;
}


