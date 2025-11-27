#ifndef Server_hpp
#define Server_hpp

#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>

#include <atomic>
#include <chrono>
#include <iostream>
#include <list>
#include <functional>
#include <memory>
#include <mutex>
#include <netdb.h>
#include <poll.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <thread>
#include <unistd.h>
#include <vector>

/*!
    \brief состояния сокета

    оповещение о статусе соединения
*/
enum socket_status {
    ACTIVE,
    DISACTIVE
};

/*!
    \brief Клиент
*/
class Client {
public:
    /*!
    \brief Конструктор
    \param[in] socket - сокет
    \param[in] addr - unix адрес сокета
    \param[in] id - id клиета у сервера
    */
    Client(int socket, sockaddr_in addr, int id);

    /*!
     \brief Диструктор
    */
    ~Client();

    /*!
     \brief Отправить сообщение
    \param [in] msg указатель на сообщение которое надо отправить
    */
    void sendData(uint8_t *buffer, size_t n);

    /*!
     \brief Вернуть соке
     \return unix сокет
    */
    int getSocket() const noexcept;

    /*!
     \brief id клиента
     \return id
    */
    int getId() const noexcept;

    /*!
     \brief порт для сервера
     \return порт
    */
    uint16_t getPort() const noexcept;

    /*!
     \brief ip сервера
     \return ip в unix формате
    */
    uint32_t getHost() const noexcept;

private:
    int m_id; /*!< id клиента*/

    int m_sock; /*!< Сокет*/

    socket_status m_status; /*!< Статус соединения*/

    struct sockaddr_in m_cliaddr; /*!< unix адрес*/
};

/*!
    \brief Сервер
*/
class Server {
public:
    /*!
     \brief Конструктор
     \param[in] port - порт
     \param[in] msgs - буффер сообщений
     \param[in] mtx - мьютекс
    */
    Server(int port, std::function<void(uint8_t*, size_t, long)> messageHandler);

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
    void sendDataTo(int cliendId, uint8_t* buffer, size_t n);

    /*!
     \brief Получить список клиентов сервера
     \return список клиентов
    */
    std::list<int> getClientsId();

private:
    /*!
     \brief Обработчик соединения с клиентами
    */
    void acceptClients();

    /*!
     \brief Обработчик сообщений
    \param [in] client - указатель на клиента
    */
    void messageHandler(const std::unique_ptr<Client> &client);

    void messageHandle(const std::unique_ptr<Client>& client);

    int m_port; /*!< Порт */

    int m_sock; /*!< Сокет*/

    int m_ids; /*!< Последний id*/

    struct sockaddr_in m_servaddr; /*!< unix адрес*/

    socket_status m_status; /*!< Состояние соединения*/

    std::atomic<bool> m_run; /*!< атомик для управления потоками*/

    std::vector<std::unique_ptr<Client>> m_clients; /*!< список клиентов*/

    std::unique_ptr<std::thread> m_acceptThread; /*!< поток для подключения клиентов */

    std::vector<std::unique_ptr<std::thread>>
        m_messageThreads; /*!< мьютекс для работы пересылки сообщений */

    std::mutex m_mtx; /*!< мьютекс для работы с потоками*/

    std::function<void(uint8_t*, size_t, long)> m_messageHandler;
};

#endif
