
#ifndef Network_hpp
#define Network_hpp

#include "Server.hpp"
#include "Message.hpp"
#include "BlockChain.hpp"
#include <utility>

class BlockChain;

/*!
    \brief Сеть

    Реализация p2p blockchain сети
*/
class Network{
public:
    /*!
     \brief Конструктор
     \param[in] clientIp - список серверов в цепи
     \param[in] port - порт
     \param[in] bc - локальная цепь блоков
    */
    Network(std::list< std::pair<std::string, int>> clientIp, int port, BlockChain *bc);

    /*!
     \brief Деструктор
    */
    ~Network();

    /*!
     \brief Запрос на обновление базы данных новыми блоками
    */
    void getBlocks();
    
    /*!
     \brief Отправить транзакцию в буффер
    */
    void sendToMempool(Transaction *tx);
    
    /*!
     \brief Создать соединение
     \param [in] host - ip адрес сервера
     \param [in] port - порт сервера
    */
    void connectTo(std::string host, int port);
    
    /*!
     \brief Получить транзакцию из буфера для создания блока
     \return укалатель на транзакцию
    */
    Transaction *getFromMempool();
    
private:
    /*!
     \brief Обработчик сообщений
    */
    void process();

    /*!
     \brief Послать сообщение noFound
     \param [in] clientId - id клиента
    */
    void noFound(int clientId);
    
    /*!
     \brief Отправить сообщение inv
     \param [in] hash - список хэшей
     \param [in] clientId - id клиента
    */
    void inv(std::array<uint32_t, 8> hash, int clientId);
    
    /*!
     \brief Послать сообщение getData
     \param [in] hashes - список хэшей
     \param [in] clientId - id клиента
    */
    void getData(std::list<std::array<uint32_t, 8>> hashes, int clientId);
    
    /*!
     \brief Послать сообщение sBlock
     \param [in] hashes - список хэшей блоков
     \param [in] clientId - id клиента
    */
    void sblock(std::list<std::array<uint32_t, 8>> hashes, int clientId);
    
    Server *m_serv;  /*!< Сервер*/
    
    BlockChain *m_bc;  /*!< Локальный blockchain */
    
    std::list<Message *> m_msgs;  /*!< Буффер сообщений*/

    std::list <std::pair<std::string, int>> m_clientsIp;  /*!< список серверов*/
    
    std::list<Transaction *> m_mempool; /*!< Буффер транзакций без блока */
    
    std::atomic<bool> m_run;  /*!< Атомик для управления потоками */

    std::mutex m_mtx;  /*!< мьютекс для управления памятью*/

    std::thread m_procThr;  /*!< поток для обработки сообщений*/

    int m_port;  /*!< порт*/
};

#endif
