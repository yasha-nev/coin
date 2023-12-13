#ifndef Server_hpp
#define Server_hpp

#include <iostream>
#include <stdio.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <thread>
#include <atomic>
#include <vector>
#include <list>
#include <chrono>
#include <mutex>
#include "Message.hpp"


/*!
    \brief состояния сокета

    оповещение о статусе соединения
*/
enum socket_status{
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
class Client{
public:
    /*!
    Конструктор
    \param[in] socket - сокет
    \param[in] addr - unix адрес сокета
    \param[in] id - id клиета у сервера
    \param[in] msgs - указатель на буффер сообщений
    */
    Client(int socket, sockaddr_in addr, int id, std::list<Message *> *msgs);
    
    /*!
    Диструктор
    */
    ~Client();
    
    /*!
    Отправить сообщение
    \param [in] msg указатель на сообщение которое надо отправить
    */
    void sendData(Message *msg);
    
    /*!
    \return сокет
    */
    int getSocket();
    
    /*!
    \return id клиета
    */
    int getId();

    /*!
    \return порт
    */
    uint16_t getPort();

    /*!
    \return ip
    */
    uint32_t getHost();
    
private:
    int m_id; /*!< id клиента*/
    
    int m_sock; /*!< Сокет*/
    
    socket_status m_status; /*!< Статус соединения*/
    
    struct sockaddr_in m_cliaddr; /*!< unix адрес*/
    
    std::list<Message *> *m_msgs; /*!< буффер сообщений*/
};

/*!
    \brief Сервер
*/
class Server{
public:
    /*!
    Конструктор
    \param[in] port - порт
    \param[in] msgs - буффер сообщений
    \param[in] mtx - мьютекс
    */
    Server(int port, std::list<Message *> *msgs, std::mutex *mtx);
    
    /*!
    Диструктор
    */
    ~Server();
    
    /*!
    Начать работу сервера
    */
    void start();
    
    /*!
    Закончить работу сервера
    */
    void stop();

    /*!
    Начать соединение
    \param [in] ip адрес клиента
    \param [in] port порт клиента
    \return статус соединения
    */
    int connectTo(std::string ip, int port);
    
    /*!
    Отправить сообщение
    \param [in] id - id клиента
    \param [in] msg - указатель на сообщение
    */
    void sendDataTo(int id, Message *msg);
    
    /*!
    \return список клиентов
    */
    std::list<int> getClientsId();

private:
    /*!
    Обработчик соединения с клиентами
    */
    void acceptClients();
    
    /*!
    Обработчик сообщений
    \param [in] client - указатель на клиента
    */
    void messageHandler(Client *client);
    
    int m_port; /*!< Порт */
    
    int m_sock; /*!< Сокет*/
    
    int m_ids; /*!< Последний id*/
    
    struct sockaddr_in m_servaddr; /*!< unix адрес*/
    
    socket_status m_status; /*!< Состояние соединения*/
    
    std::atomic<bool> m_run; /*!< атомик для управления потоками*/
    
    std::vector<Client *> m_clients; /*!< список клиентов*/
    
    std::list<Message *> *m_msgs; /*!< буффер сообщений*/
    
    std::thread *m_acceptThread;
    
    std::vector<std::thread *> m_messageThreads;
    
    std::mutex m_mtx; /*!< мьютекс для работы с потоками*/
    
    std::mutex *m_msgMtx; /*!< Мьютекс для обработки сообщений*/
};

#endif
