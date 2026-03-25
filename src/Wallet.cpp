#include "Wallet.hpp"

Wallet::Wallet(BlockChain& bc, Network& net):
    m_bc(bc),
    m_net(net) {

    auto rsaProcessor = IRsaProcessor::create("./");

    m_privkey = rsaProcessor->getPrivateKeyBase58();
    m_pubkey = rsaProcessor->getPublicKeyBase58();
}

Wallet::~Wallet() {
}

std::string Wallet::getAddres() {
    std::string sum = checkSum(m_pubkey);

    std::string src = version() + m_pubkey + sum;

    auto hashEncoder = IHashEncoder::create();

    Hash ripmd = hashEncoder->ripemd160Hash(src);
    std::string ripmdstr = hashEncoder->ripemd160HashToString(ripmd);
    std::string res = hashEncoder->encodeBase58(ripmdstr);

    res.pop_back();

    return res;
}

uint64_t Wallet::getBalance() {
    return m_bc.getBalance(m_pubkey, getAddres());
}

std::string Wallet::checkSum(const std::string& payload) {
    (void) payload;
    std::string result;
    return result;
}

std::string Wallet::version() {
    return "0";
}

void Wallet::createTransaction(const std::string& address, int value) {
    int rest;

    std::list<TXInput> inputs = m_bc.getInputs(m_pubkey, getAddres(), value, &rest);
    if(inputs.empty()) {
        std::cout << "not money" << std::endl;
        return;
    }

    Transaction tx = TransactionFactory::createSimple(
        m_bc.getPastTransactionId() + 1, getAddres(), address, value, inputs, rest);

    m_net.sendToMempool(tx);
}
