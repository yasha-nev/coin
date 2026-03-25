#include "BlockChain.hpp"

#include <cstdint>
#include <memory>

BlockChain::BlockChain(std::unique_ptr<IDataBase> db):
    m_db(std::move(db)) {

    m_db->connect();

    if(std::filesystem::exists(DBPATH)) {
        std::optional<Hash> hash = m_db->getCurrentHash();

        if(hash.has_value()) {
            m_cur_hash = *hash;
        }

    } else {
        Block block = genesisBlock();
        m_cur_hash = block.getHash();
        m_db->putBlock(block);
    }
}

void BlockChain::createBlock(uint64_t time, const std::list<Transaction>& tx) {
    Block block_n = newBlock(time, tx, m_cur_hash);

    m_db->putBlock(block_n);
    m_cur_hash = block_n.getHash();
}

Block BlockChain::genesisBlock() {
    Hash zero_hash = createZeroHash();
    std::list<Transaction> txList;

    uint64_t id = 0;
    std::string pubkey = "BcVtjVsKHxRNbne4MKCBxaQbJyoQsZQ8JTb1m3mAygwkyCLFP6Jvmrrf";

    Transaction transaction = TransactionFactory::createCoinBase(id, pubkey);

    txList.push_back(transaction);

    Block block = newBlock(0, txList, zero_hash);
    m_cur_hash = block.getHash();

    return block;
}

void BlockChain::printChain() const noexcept {
    Hash hash = m_cur_hash;
    std::string blc;

    while(!checkZeroHash(hash)) {
        std::optional<Block> block = getBlock(hash);

        if(!block.has_value()) {
            break;
        }

        hash = block->getPrevBlockHash();
        block->print();
    }
}

std::list<Hash> BlockChain::getHashesBefore(const Hash& curHash) const noexcept {
    std::list<Hash> lst;

    Hash hash = m_cur_hash;
    std::string blc;

    if(curHash == hash) {
        return lst;
    }

    while(!checkZeroHash(hash)) {
        if(hash == curHash) {
            break;
        }

        std::optional<Block> block = getBlock(hash);

        if(!block.has_value()) {
            break;
        }

        lst.push_front(hash);

        hash = block->getPrevBlockHash();
    }

    return lst;
}

Block BlockChain::newBlock(
    uint64_t time,
    const std::list<Transaction>& tx,
    const Hash& prevHashBlock) {
    Block block = Block(time, tx, prevHashBlock, Hash(), 0);

    ProofOfWork pow(block);
    pow.Run();

    return block;
}

void BlockChain::putBlock(const Block& block) {
    m_db->putBlock(block);
    m_cur_hash = block.getHash();
}

std::optional<Block> BlockChain::getBlock(const Hash& hash) const noexcept {
    return m_db->getBlockByHash(hash);
}

std::optional<Block> BlockChain::getPastBlock() const noexcept {
    return m_db->getBlockByHash(m_cur_hash);
}

const Hash& BlockChain::getPastBlockHash() const noexcept {
    return m_cur_hash;
}

uint64_t BlockChain::getPastTransactionId() const noexcept {
    auto block = m_db->getBlockByHash(m_cur_hash);

    const std::list<Transaction>& transactions = block->getTransactions();

    if(transactions.empty()) {
        return -1;
    }

    return transactions.back().getId();
}

TXOutput* getOutputs(const std::string& from, int value, int* count) {
    (void) from;
    (void) value;
    (void) count;
    return nullptr;
}

uint64_t BlockChain::getBalance(const std::string& pubkey, const std::string& address)
    const noexcept {
    int sum = 0;
    Hash hash = m_cur_hash;

    struct outId {
        uint64_t id;
        int outIndex;
    };

    std::vector<struct outId> outIds;

    std::string blc;

    while(!checkZeroHash(hash)) {
        std::optional<Block> block = getBlock(hash);
        hash = block->getPrevBlockHash();

        const std::list<Transaction>& txList = block->getTransactions();

        for(auto& tx: txList) {

            std::vector<outId> sup;

            sup.insert(outIds.begin(), outIds.end(), outIds.end());

            int i = 0;
            for(const auto& out: tx.getOutputs()) {
                if(out.getAddress() != address) {
                    i++;
                    continue;
                }

                bool flag = true;
                for(size_t k = 0; k < sup.size(); k++) {
                    if(outIds[k].outIndex == i) {
                        flag = false;
                    }
                }

                if(flag) {
                    sum += out.getValue();
                }
                i++;
            }

            for(const auto& in: tx.getInputs()) {
                if(in.getPublicKey() != pubkey) {
                    continue;
                }

                outId oi = { in.getTransactionId(), in.getOutIndex() };
                outIds.push_back(oi);
            }
        }
    }

    return sum;
}

std::list<TXInput> BlockChain::getInputs(
    const std::string& pubkey,
    const std::string& address,
    int value,
    int* rest) const noexcept {
    std::list<TXInput> ls;

    int sum = 0;

    Hash hash = m_cur_hash;

    *rest = 0;

    struct outId {
        uint64_t id;
        int outIndex;
    };

    std::vector<struct outId> outIds;

    std::string blc;

    while(!checkZeroHash(hash)) {
        std::optional<Block> block = getBlock(hash);
        if(!block.has_value()) {
            break;
        }

        hash = block->getPrevBlockHash();

        const std::list<Transaction>& txList = block->getTransactions();

        for(const auto& tx: txList) {
            std::vector<outId> sup;

            sup.insert(outIds.begin(), outIds.end(), outIds.end());

            int i = 0;
            for(const auto& out: tx.getOutputs()) {
                if(out.getAddress() != address) {
                    i++;
                    continue;
                }

                bool flag = true;

                for(size_t k = 0; k < sup.size(); k++) {
                    if(outIds[k].outIndex == i) {
                        flag = false;
                    }
                }

                if(flag) {
                    ls.push_back(TXInput(tx.getId(), i, pubkey));
                    sum += out.getValue();
                }
                i++;
            }

            for(const auto& in: tx.getInputs()) {
                if(in.getPublicKey() == pubkey) {
                    outId oi = { in.getTransactionId(), in.getOutIndex() };
                    outIds.push_back(oi);
                }
            }

            if(sum >= value) {
                break;
            }
        }
    }

    if(sum < value) {
        ls.clear();
    }
    *rest = sum - value;

    return ls;
}
