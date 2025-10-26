#ifndef Server_hpp
#define Server_hpp

#include "Message.hpp"

#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>

#include <atomic>
#include <chrono>
#include <iostream>
#include <list>
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
    sock_err,    /*!< Ошибка создания сокета*/
    bind_err,    /*!< Ошибка bind сокета*/
    listen_err,  /*!< Ошибка прослушивания сокета*/
    connect_err, /*!< Ошибка установки соединения*/
    start,       /*!< Сокет работает */
    stop         /*!< Сокет закрыт*/
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
    \param[in] msgs - указатель на буффер сообщений
    */
    Client(int socket, sockaddr_in addr, int id, std::list<std::unique_ptr<Message>>* msgs);

    /*!
     \brief Диструктор
    */
    ~Client();

    /*!
     \brief Отправить сообщение
    \param [in] msg указатель на сообщение которое надо отправить
    */
    void sendData(Message* msg);

    /*!
     \brief Вернуть соке
     \return unix сокет
    */
    int getSocket();

    /*!
     \brief id клиента
     \return id
    */
    int getId();

    /*!
     \brief порт для сервера
     \return порт
    */
    uint16_t getPort();

    /*!
     \brief ip сервера
     \return ip в unix формате
    */
    uint32_t getHost();

private:
    int m_id; /*!< id клиента*/

    int m_sock; /*!< Сокет*/

    socket_status m_status; /*!< Статус соединения*/

    struct sockaddr_in m_cliaddr; /*!< unix адрес*/

    std::list<std::unique_ptr<Message>>* m_msgs; /*!< буффер сообщений*/
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
    Server(int port, std::list<std::unique_ptr<Message>>* msgs, std::mutex* mtx);

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
    int connectTo(std::string ip, int port);

    /*!
     \brief Отправить сообщение
     \param [in] id - id клиента
     \param [in] msg - указатель на сообщение
    */
    void sendDataTo(int id, Message* msg);

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
    void messageHandler(std::shared_ptr<Client> client);

    void messageHandle(const std::shared_ptr<Client>& client);

    int m_port; /*!< Порт */

    int m_sock; /*!< Сокет*/

    int m_ids; /*!< Последний id*/

    struct sockaddr_in m_servaddr; /*!< unix адрес*/

    socket_status m_status; /*!< Состояние соединения*/

    std::atomic<bool> m_run; /*!< атомик для управления потоками*/

    std::vector<std::shared_ptr<Client>> m_clients; /*!< список клиентов*/

    std::list<std::unique_ptr<Message>>* m_msgs; /*!< буффер сообщений*/

    std::unique_ptr<std::thread> m_acceptThread; /*!< поток для подключения клиентов */

    std::vector<std::unique_ptr<std::thread>>
        m_messageThreads; /*!< мьютекс для работы пересылки сообщений */

    std::mutex m_mtx; /*!< мьютекс для работы с потоками*/

    std::mutex* m_msgMtx; /*!< Мьютекс для обработки сообщений*/
};

#endif
