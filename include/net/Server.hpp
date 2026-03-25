#pragma once

#include "Client.hpp"

#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>

#include <atomic>
#include <chrono>
#include <functional>
#include <iostream>
#include <list>
#include <memory>
#include <mutex>
#include <netdb.h>
#include <poll.h>
#include <span>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <thread>
#include <unistd.h>
#include <vector>

#define DEFAULT_CLIENTS_COUNT 100

/*!
    \brief состояния сокета

    оповещение о статусе соединения
*/
enum socket_status { ACTIVE, DISACTIVE };

/*!
    \brief Сервер
*/
class Server {
public:
    /*!
     \brief Конструктор
     \param[in] port - порт
     \param[in] msgs - буффер сообщений
    */
    Server(int port, std::function<void(std::span<const std::byte>, ClientID)> messageHandler);

    /*!
     \brief Диструктор
    */
    ~Server();

    /*!
     \brief Начать работу сервера
    */
    void start();

    /*!
     \brief Закончить работу сервера
    */
    void stop();

    /*!
     \brief Начать соединение
     \param [in] ip адрес клиента
     \param [in] port порт клиента
     \return статус соединения
    */
    int connectTo(const std::string& ip, int port);

    /*!
     \brief Отправить сообщение
     \param [in] clientId - id клиента
     \param [in] buffer - массив байтов сообщения
     \param [in] n - размер сообщения
    */
    void sendDataTo(ClientID cliendId, std::span<const std::byte> buffer);

    /*!
     \brief Получить список клиентов сервера
     \return список клиентов
    */
    std::list<ClientID> getClientsId();

private:
    void acceptClients();

    void readReceivedMessages();

    bool addClient(int clientSocket, sockaddr_in clientAddr);

    socket_status m_status;

    int m_port;

    int m_sock;

    struct sockaddr_in m_servaddr;

    std::vector<struct pollfd> m_pollDescriptors;

    std::atomic<bool> m_run; /*!< атомик для управления потоками*/

    std::vector<std::unique_ptr<Client>> m_clients; /*!< список клиентов*/

    std::unique_ptr<std::thread> m_acceptThread; /*!< поток для подключения клиентов */

    std::mutex m_mtx; /*!< мьютекс для работы с потоками*/

    std::function<void(std::span<const std::byte>, long)> m_messageHandler;
};
