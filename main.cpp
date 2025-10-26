#include "BlockChain.hpp"
#include "LeveldbDataBase.hpp"
#include "Mainer.hpp"
#include "Message.hpp"
#include "Wallet.hpp"

void CLI(BlockChain& bc, Wallet wallet, Network& net) {
    std::string command;

    std::cout << "user      - get wallet address \n";
    std::cout << "send      - create and send Transaction \n";
    std::cout << "balance   - get wallet balance \n";
    std::cout << "connect   - connect to user of network \n";
    std::cout << "print     - print the blockchain \n";
    std::cout << "updatedb  - obtain blocks from network users \n";
    std::cout << "quit      - exit from programm \n";

    while(true) {
        std::cout << "CLI>";
        std::cin >> command;

        if(command == "user") {
            std::cout << wallet.getAddres() << "\n";

        } else if(command == "send") {
            std::string to;
            int value;

            std::cout << "to: ";
            std::cin >> to;
            std::cout << "value: ";
            std::cin >> value;

            wallet.createTransaction(to, value);

        } else if(command == "balance") {
            std::cout << "=================Balance=================\n";
            std::cout << "user: " << wallet.getBalance() << "\n";

        } else if(command == "print") {
            bc.printChain();

        } else if(command == "connect") {
            std::string host;
            int port;

            std::cout << "host: ";
            std::cin >> host;
            std::cout << "port: ";
            std::cin >> port;

            net.connectTo(host, port);

        } else if(command == "updatedb") {
            net.getBlocks();

        } else if(command == "quit") {
            break;
        } else {
            std::cout << "user      - get wallet address \n";
            std::cout << "send      - create and send Transaction \n";
            std::cout << "balance   - get wallet balance \n";
            std::cout << "connect   - connect to user of network \n";
            std::cout << "print     - print the blockchain \n";
            std::cout << "updatedb  - obtain blocks from network users \n";
            std::cout << "quit      - exit from programm \n";
        }
    }
}

void asWallet(BlockChain& bc, Network& net) {
    Wallet wallet(&bc, &net);
    CLI(bc, wallet, net);
}

void asMainer(BlockChain& bc, Network& net) {
    Mainer mainer(&bc, &net);
    std::string command;
    std::cout << "quit      - exit from programm \n";

    while(true) {
        std::cout << "CLI>";
        std::cin >> command;

        if(command == "quit") {
            return;
        } else {
            std::cout << "quit      - exit from programm \n";
        }
    }
}

int main(int argc, const char* argv[]) {
    if(argc < 3) {
        std::cout << "Enter the port and operating mode\n";
        std::cout << "-w wallet mode, -m mainer mode\n";
        std::cout << "example ./main 5060 -w" << std::endl;
        return 0;
    }

    int port = std::stoi(argv[1]);
    std::string res = argv[2];

    auto db = std::make_unique<LeveldbDataBase>();

    BlockChain bc(std::move(db));

    Network net(std::list<std::pair<std::string, int>>(), port, &bc);
    if(res == "-w") {
        asWallet(bc, net);
    } else if(res == "-m") {
        asMainer(bc, net);
    } else {
    }

    return 0;
}
