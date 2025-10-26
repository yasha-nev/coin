#include "Mainer.hpp"

#include "BlockChain.hpp"
#include "Message.hpp"

using namespace std;

int main(int argc, const char* argv[]) {
    if(argc < 2) {
        return 0;
    }
    int port = stoi(argv[1]);

    BlockChain bc;
    Network net(std::list<std::pair<std::string, int>>(), port, &bc);
    Mainer mainer(&bc, &net);

    while(true) {
    }

    return 0;
}
