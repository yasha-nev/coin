
#ifndef Network_hpp
#define Network_hpp

#include "BlockChain.hpp"
#include "Message.hpp"
#include "Server.hpp"

#include <utility>

class BlockChain;

/*!
    \brief Сеть

    Реализация p2p blockchain сети
*/
class Network {
public:
    /*!
     \brief Конструктор
     \param[in] clientIp - список серверов в цепи
     \param[in] port - порт
     \param[in] bc - локальная цепь блоков
    */
    Network(int port, BlockChain &bc);

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
    void sendToMempool(std::unique_ptr<Transaction> tx);

    /*!
     \brief Создать соединение
     \param [in] host - ip адрес сервера
     \param [in] port - порт сервера
    */
    void connectTo(const std::string& host, int port);

    /*!
     \brief Получить транзакцию из буфера для создания блока
     \return укалатель на транзакцию
    */
    std::unique_ptr<Transaction> getFromMempool();

private:

    /*!
     \brief Обработчик сообщений
    */
    void messageHandler(uint8_t *buffer, size_t n, long clientId);

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
    void inv(std::array<uint8_t, 32> hash, int clientId);

    /*!
     \brief Послать сообщение getData
     \param [in] hashes - список хэшей
     \param [in] clientId - id клиента
    */
    void getData(std::list<std::array<uint8_t, 32>> hashes, int clientId);

    /*!
     \brief Послать сообщение sBlock
     \param [in] hashes - список хэшей блоков
     \param [in] clientId - id клиента
    */
    void sblock(std::list<std::array<uint8_t, 32>> hashes, int clientId);

    std::unique_ptr<Server> m_serv;

    BlockChain &m_bc;

    std::list<std::pair<std::string, int>> m_clientsIp;

    std::list<std::unique_ptr<Transaction>> m_mempool;

    std::mutex m_mtx;
};

#endif
