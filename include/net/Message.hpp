#ifndef Message_hpp
#define Message_hpp

#define STARTSTR "f9beb4d9"

#include <inttypes.h>
#include <string.h>
#include <list>
#include <array>
#include "Block.hpp"
#include "sha256.hpp"

enum MsgTypes{
    gBlocks = 0,
    gData   = 1,
    sBlock  = 2,
    Inv     = 3,
    noFound = 4
};

class Message{
public:
    
    virtual ~Message();
    
    virtual void parse(uint8_t *data, size_t size) = 0;
    
    virtual uint8_t *toByte(size_t &size) = 0;
    
    virtual uint8_t getCommand(){
        return m_comm;
    }
    
    virtual void setClientId(int id){
        clientId = id;
    }
    
    virtual int getClientId(){
        return clientId;
    }
    
protected:
    int clientId = -1;
    uint8_t m_ver = 0;
    size_t m_size;
    uint8_t m_comm;
};

/*               Message Header
 *  ------------------------------------------
 * | start_string | Command | Size | Checksum |
 *  ------------------------------------------
 *                   Payload
 *  --------------------------------------------
 * | Prot vers         |    Header Hash Count 1 |
 * |--------------------------------------------|
 * | Header heash                               |
 * | Stopping hash 00000000000000000000000000000|
 *  --------------------------------------------
 */

class GetBlocksMsg : public Message{
public:
    GetBlocksMsg();
    
    GetBlocksMsg(std::list<std::array<uint32_t, 8>> hashes);
    
    virtual void parse(uint8_t *data, size_t size) override;
    
    virtual uint8_t *toByte(size_t &size) override;
    
    std::list<std::array<uint32_t, 8>> getHashes();

private:
    std::list<std::array<uint32_t, 8>> m_hashes;
    
};

/*               Message Header
 *  ------------------------------------------
 * | start_string | Command | Size | Checksum |
 *  ------------------------------------------
 *                   Payload
 *  --------------------------------------------
 * |           number of inv(500)               |
 * |--------------------------------------------|
 * | type        unique identif(header hash)    |
 * | block         header hash                  |
 * | block         header hash                  |
 *  --------------------------------------------
 */

enum InvTypes{
    iBlock = 0,
    iTx = 1
};

class InvMsg : public Message{
public:
    InvMsg();
    
    InvMsg(InvTypes type, std::list<std::array<uint32_t, 8>> hashes);
    
    virtual void parse(uint8_t *data, size_t size) override;
    
    virtual uint8_t *toByte(size_t &size) override;
    
    std::list<std::array<uint32_t, 8>> getHashes();

private:
    
    uint8_t m_type;
    
    std::list<std::array<uint32_t, 8>> m_hashes;
    
};

/*               Message Header
 *  ------------------------------------------
 * | start_string | Command | Size | Checksum |
 *  ------------------------------------------
 *                   Payload
 *  --------------------------------------------
 * |           number of objects(128)           |
 * |--------------------------------------------|
 * | type        unique identif(header hash)    |
 * | block         header hash                  |
 * | block         header hash                  |
 *  --------------------------------------------
 */

enum DataTypes{
    dBlock = 0,
    dTx = 1
};

class GetDataMsg : public Message{
public:
    GetDataMsg();
    
    GetDataMsg(DataTypes type, std::list<std::array<uint32_t, 8>> hashes);
    
    virtual void parse(uint8_t *data, size_t size) override;
    
    virtual uint8_t *toByte(size_t &size) override;
    
    std::list<std::array<uint32_t, 8>> getHashes();

private:
    uint8_t m_type;
    std::list<std::array<uint32_t, 8>> m_hashes;
    
};

/*               Message Header
 *  ------------------------------------------
 * | start_string | Command | Size | Checksum |
 *  ------------------------------------------
 *                   Payload
 *  --------------------------------------------
 * |              serilized block               |
 *  --------------------------------------------
 */


class BlockMsg : public Message{
public:
    
    BlockMsg();
    
    BlockMsg(Block *blocks);
    
    virtual void parse(uint8_t *data, size_t size) override;
    
    virtual uint8_t *toByte(size_t &size) override;
    
    Block *getBlock();

private:
    Block *m_block;
    
};


/*               Message Header
 *  ------------------------------------------
 * | start_string | Command | Size | Checksum |
 *  ------------------------------------------
 */


class NoFoundMsg : public Message{
public:
    
    NoFoundMsg();
    
    virtual void parse(uint8_t *data, size_t size) override;
    
    virtual uint8_t *toByte(size_t &size) override;

};

#endif
