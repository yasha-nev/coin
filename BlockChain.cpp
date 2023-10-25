#include "BlockChain.hpp"

static inline bool file_exist (const std::string& name) {
    if (FILE *file = fopen(name.c_str(), "r")) {
        fclose(file);
        return true;
    } else {
        return false;
    }
}

BlockChain::BlockChain(){
    if (file_exist("/Users/yasha_nev/projects/BlockChain/BlockChain/testdb")){
        leveldb::Options options;
        leveldb::DB::Open(options, "/Users/yasha_nev/projects/BlockChain/BlockChain/testdb", &m_db);
        
        string hash;
        m_db->Get(leveldb::ReadOptions(), "l", &hash);
        memcpy(cur_hash, hash.c_str(), sizeof(uint32_t) * 8);
    }
    else{
        leveldb::Options options;
        options.create_if_missing = true;
        leveldb::DB::Open(options, "/Users/yasha_nev/projects/BlockChain/BlockChain/testdb", &m_db);
        
        Block *block = genesisBlock();
        size_t block_size = 0;
        uint8_t *enc_block = block->encode(&block_size);
        memcpy(cur_hash, block->getHash(), sizeof(uint32_t) * 8);
        
        leveldb::Slice key((char *) block->getHash(), sizeof(uint32_t) * 8);
        leveldb::Slice value((char *)enc_block, block_size);
        leveldb::Slice value_hash((char *)block->getHash(), block_size);
        
        m_db->Put(leveldb::WriteOptions(), key, value);
        m_db->Put(leveldb::WriteOptions(), "l", value_hash);
        
        delete block;
        delete[] enc_block;
    }
}

BlockChain::~BlockChain(){
    delete m_db;
}

void BlockChain::addBlock(string data){
    Block *block_n = newBlock(data, cur_hash);
    
    size_t block_size = 0;
    uint8_t *enc_block = block_n->encode(&block_size);
    
    leveldb::Slice key((char *) block_n->getHash(), sizeof(uint32_t) * 8);
    leveldb::Slice value((char *)enc_block, block_size);
    m_db->Put(leveldb::WriteOptions(), key, value);
    m_db->Put(leveldb::WriteOptions(), "l", key);
    
    memcpy(cur_hash, block_n->getHash(), sizeof(uint32_t) * 8);
}

Block* BlockChain::genesisBlock(){
    uint32_t zero_hash[8] = {0, 0, 0, 0, 0, 0, 0, 0};
    Block *block_n = newBlock("genesis block", zero_hash);
    array2String(block_n->getHash());
    memcpy(cur_hash, block_n->getHash(), sizeof(uint32_t) * 8);
    return block_n;
}

void BlockChain::printChain(){
    leveldb::Iterator *it = m_db->NewIterator(leveldb::ReadOptions());
    for (it->SeekToFirst(); it->Valid(); it->Next()){
        if (it->key().ToString() == "l"){
            continue;
        }
        Block *block = decode((uint8_t *) it->value().ToString().c_str());
        block->print();
        delete block;
    }
    delete it;
}

Block *BlockChain::newBlock(string Data, uint32_t *prevHashBlock){
    Block *block_n = new Block( static_cast<uint64_t>(std::time(nullptr)),
                               Data,
                               prevHashBlock,
                               new uint32_t[8],
                               0);
    ProofOfWork pow(block_n);
    pow.Run();
    return block_n;
}

Block *BlockChain::getBlock(uint32_t *hash){
    string dec_block;
    leveldb::Slice key((char *) hash, sizeof(uint32_t) * 8);
    m_db->Get(leveldb::ReadOptions(), key, &dec_block);
    return decode((uint8_t *) dec_block.c_str());
}
