#ifndef Client_hpp
#define Client_hpp

#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>

#include <netdb.h>
#include <span>
#include <unistd.h>

using ClientID = long;

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
    Client(int socket, sockaddr_in addr, ClientID id);

    /*!
     \brief Диструктор
    */
    ~Client();

    /*!
     \brief Отправить сообщение
    \param [in] msg указатель на сообщение которое надо отправить
    */
    void sendData(std::span<const std::byte> buffer);

    /*!
     \brief Вернуть соке
     \return unix сокет
    */
    int getSocket() const noexcept;

    /*!
     \brief id клиента
     \return id
    */
    ClientID getId() const noexcept;

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
    ClientID m_id; /*!< id клиента*/

    int m_sock; /*!< Сокет*/

    struct sockaddr_in m_cliaddr; /*!< unix адрес*/
};

#endif
