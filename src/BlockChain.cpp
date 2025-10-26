#include "BlockChain.hpp"

#include <cstdint>
#include <memory>

bool file_exist(const std::string& path) {
    if(FILE* file = fopen(path.c_str(), "r")) {
        fclose(file);
        return true;
    } else {
        return false;
    }
}

static bool checkFirstBlock(const std::array<uint32_t, 8>& hash) {
    bool flag = true;

    for(int i = 0; i < 8; i++) {
        if(hash[i] != 0) {
            flag = false;
        }
    }

    return flag;
}

BlockChain::BlockChain(std::unique_ptr<IDataBase> db):
    m_db(std::move(db)) {
    if(file_exist(DBPATH)) {
        m_db->connect();
        m_cur_hash = m_db->getCurrentHash();
    } else {
        m_db->connectIfexist();
        auto block = genesisBlock();
        m_cur_hash = block->getHash();

        m_db->putBlock(block);
    }
}

void BlockChain::createBlock(uint64_t time, const std::list<Transaction>& tx) {
    auto block_n = newBlock(time, tx, m_cur_hash);

    m_db->putBlock(block_n);
    m_cur_hash = block_n->getHash();
}

std::unique_ptr<Block> BlockChain::genesisBlock() {
    std::array<uint32_t, 8> zero_hash = { 0, 0, 0, 0, 0, 0, 0, 0 };
    std::list<Transaction> lst;

    uint64_t id = 0;
    std::string pubkey = "BcVtjVsKHxRNbne4MKCBxaQbJyoQsZQ8JTb1m3mAygwkyCLFP6Jvmrrf";

    lst.push_back(CoinBaseTransaction(id, pubkey));

    std::unique_ptr<Block> block_n = newBlock(0, lst, zero_hash);
    m_cur_hash = block_n->getHash();

    return block_n;
}

void BlockChain::printChain() const noexcept {
    std::array<uint32_t, 8> hash;
    hash = m_cur_hash;
    std::string blc;

    while(!checkFirstBlock(hash)) {
        auto block = getBlock(hash);
        hash = block->getPrevBlockHash();
        block->print();
    }
}

std::list<std::array<uint32_t, 8>> BlockChain::getHashesBefore(
    std::array<uint32_t, 8> curHash) const noexcept {
    std::list<std::array<uint32_t, 8>> lst;

    std::array<uint32_t, 8> hash = m_cur_hash;
    std::string blc;

    if(curHash == hash) {
        return lst;
    }

    while(!checkFirstBlock(hash)) {
        if(hash == curHash) {
            break;
        }

        auto block = getBlock(hash);

        if(!block) {
            break;
        }

        lst.push_front(hash);

        hash = block->getPrevBlockHash();
    }

    return lst;
}

std::unique_ptr<Block> BlockChain::newBlock(
    uint64_t time,
    const std::list<Transaction>& tx,
    const std::array<uint32_t, 8>& prevHashBlock) {
    auto block_n = std::make_unique<Block>(time, tx, prevHashBlock, std::array<uint32_t, 8>(), 0);

    ProofOfWork pow(block_n.get());
    pow.Run();

    return std::move(block_n);
}

void BlockChain::putBlock(std::unique_ptr<Block>& block) {
    m_db->putBlock(block);
    m_cur_hash = block->getHash();
}

std::unique_ptr<Block> BlockChain::getBlock(const std::array<uint32_t, 8>& hash) const noexcept {
    return m_db->getBlockByHash(hash);
}

std::unique_ptr<Block> BlockChain::getPastBlock() const noexcept {
    return m_db->getBlockByHash(m_cur_hash);
}

const std::array<uint32_t, 8>& BlockChain::getPastBlockHash() const noexcept {
    return m_cur_hash;
}

uint64_t BlockChain::getPastTransactionId() const noexcept {
    auto block = m_db->getBlockByHash(m_cur_hash);

    return block->m_tx.back().m_id;
}

TXOutput* getOutputs(const std::string& from, int value, int* count) {
    // toDO
    return nullptr;
}

uint64_t BlockChain::getBalance(const std::string& pubkey, const std::string& address)
    const noexcept {
    int sum = 0;
    std::array<uint32_t, 8> hash;

    struct outId {
        uint64_t id;
        int outIndex;
    };

    std::vector<struct outId> outIds;

    hash = m_cur_hash;
    std::string blc;

    while(!checkFirstBlock(hash)) {
        auto block = getBlock(hash);
        hash = block->getPrevBlockHash();

        std::list<Transaction> txList = block->getTransaction();

        for(auto& tx: txList) {

            std::vector<outId> sup;

            for(size_t l = 0; l < outIds.size(); l++) {
                sup.push_back(outIds[l]);
            }

            for(size_t i = 0; i < tx.m_out.size(); i++) {
                if(tx.m_out[i].m_address == address) {

                    bool flag = true;

                    for(size_t k = 0; k < sup.size(); k++) {
                        if(outIds[k].outIndex == i) {
                            flag = false;
                        }
                    }

                    if(flag) {
                        sum += tx.m_out[i].m_value;
                    }
                }
            }

            for(size_t i = 0; i < tx.m_in.size(); i++) {
                if(tx.m_in[i].m_pubkey == pubkey) {

                    outId oi = { tx.m_in[i].m_tranId, tx.m_in[i].m_outIndex };

                    outIds.push_back(oi);
                }
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

    std::array<uint32_t, 8> hash;

    *rest = 0;

    struct outId {
        uint64_t id;
        int outIndex;
    };

    std::vector<struct outId> outIds;

    hash = m_cur_hash;
    std::string blc;

    while(!checkFirstBlock(hash)) {
        auto block = getBlock(hash);
        hash = block->getPrevBlockHash();

        std::list<Transaction> txList = block->getTransaction();

        for(const auto& tx: txList) {

            std::vector<outId> sup;

            for(size_t l = 0; l < outIds.size(); l++) {
                sup.push_back(outIds[l]);
            }

            for(int i = 0; i < tx.m_out.size(); i++) {
                if(tx.m_out[i].m_address == address) {

                    bool flag = true;

                    for(size_t k = 0; k < sup.size(); k++) {
                        if(outIds[k].outIndex == i) {
                            flag = false;
                        }
                    }

                    if(flag) {
                        ls.push_back(TXInput(tx.m_id, i, pubkey));
                        sum += tx.m_out[i].m_value;
                    }
                }
            }

            for(size_t i = 0; i < tx.m_in.size(); i++) {
                if(tx.m_in[i].m_pubkey == pubkey) {

                    outId oi = { tx.m_in[i].m_tranId, tx.m_in[i].m_outIndex };

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
