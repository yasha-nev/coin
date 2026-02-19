
#ifndef Network_hpp
#define Network_hpp

#include "BlockChain.hpp"
#include "Message.hpp"
#include "Server.hpp"

#include <cstddef>
#include <optional>
#include <span>
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
    Network(int port, BlockChain& bc);

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
    void sendToMempool(const Transaction& tx);

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
    std::optional<Transaction> getFromMempool();

private:
    /*!
     \brief Обработчик сообщений
    */
    void messageHandler(std::span<const std::byte> buffer, ClientID lientId);

    /*!
     \brief Послать сообщение noFound
     \param [in] clientId - id клиента
    */
    void noFound(ClientID clientId);

    /*!
     \brief Отправить сообщение inv
     \param [in] hash - список хэшей
     \param [in] clientId - id клиента
    */
    void inv(const Hash& hash, ClientID clientId);

    /*!
     \brief Послать сообщение getData
     \param [in] hashes - список хэшей
     \param [in] clientId - id клиента
    */
    void getData(const std::list<Hash>& hashes, ClientID clientId);

    /*!
     \brief Послать сообщение sBlock
     \param [in] hashes - список хэшей блоков
     \param [in] clientId - id клиента
    */
    void sblock(const std::list<Hash>& hashes, ClientID clientId);

    std::unique_ptr<Server> m_serv;

    BlockChain& m_bc;

    std::list<std::pair<std::string, int>> m_clientsIp;

    std::list<Transaction> m_mempool;

    std::mutex m_mtx;
};

#endif
