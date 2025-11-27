#include "Server.hpp"

#define BUFLEN 1024
#define MAX_CLIENTS 10

Client::Client(int socket, sockaddr_in addr, int id) {
    m_sock = socket;
    m_cliaddr = addr;
    m_id = id;
}

Client::~Client() {
    if(m_sock == -1) {
        return;
    }
    close(m_sock);
}

int Client::getSocket() const noexcept {
    return m_sock;
}

uint16_t Client::getPort() const noexcept {
    return m_cliaddr.sin_port;
}

uint32_t Client::getHost() const noexcept {
    return m_cliaddr.sin_addr.s_addr;
}

int Client::getId() const noexcept {
    return m_id;
}

void Client::sendData(uint8_t *buffer, size_t n) {
    send(m_sock, buffer, n, 0);
}

Server::Server(int port, std::function<void(uint8_t*, size_t, long)> messageHandler) {
    m_status = socket_status::DISACTIVE;
    m_sock = -1;
    m_port = port;
    m_ids = 0;
    m_messageHandler = messageHandler;
}

Server::~Server() {
    stop();
}

void Server::stop() {
    if(m_status == socket_status::DISACTIVE) {
        return;
    }
    close(m_sock);
    m_run.store(false, std::memory_order_relaxed);

    if(m_acceptThread) {
        for(int i = 0; i < m_messageThreads.size(); i++) {
            m_messageThreads[i]->join();
        }
        m_acceptThread->join();
    }

    m_status = socket_status::DISACTIVE;
}

void Server::start() {
    if(m_status == socket_status::ACTIVE) {
        return;
    }

    m_sock = socket(AF_INET, SOCK_STREAM, 0);

    if(m_sock == -1) {
        throw std::runtime_error("Failed to create socket");
        return;
    }

    bzero(&m_servaddr, sizeof(sockaddr));

    m_servaddr.sin_family = AF_INET;
    m_servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    m_servaddr.sin_port = htons(m_port);

    if(bind(m_sock, (sockaddr*) &m_servaddr, sizeof(sockaddr)) != 0) {
        throw std::runtime_error("Failed to bind socket");
    }

    if((listen(m_sock, MAX_CLIENTS)) != 0) {
        throw std::runtime_error("Failed to listen");
    }

    m_status = socket_status::ACTIVE;
    m_run.store(true, std::memory_order_relaxed);
    m_acceptThread = std::make_unique<std::thread>(&Server::acceptClients, this);
}

void Server::acceptClients() {
    if(m_status != socket_status::ACTIVE) {
        return;
    }

    using namespace std::chrono_literals;

    struct pollfd fds[MAX_CLIENTS + 1];
    fds[0].fd = this->m_sock;
    fds[0].events = POLLIN;

    for(int i = 1; i <= MAX_CLIENTS; i++) {
        fds[i].fd = -1;
        fds[i].events = POLLIN;
    }

    while(m_run.load(std::memory_order_relaxed)) {
        int poll_result = poll(fds, MAX_CLIENTS + 1, -1);
        if(poll_result == -1) {
            break;
        }

        if(fds[0].revents & POLLIN) {
            sockaddr_in clientAddr;
            int len = sizeof(sockaddr);

            int clientSocket = accept(m_sock, (sockaddr*) &clientAddr, (socklen_t*) &len);
            if(clientSocket == -1) {
                continue;
            }

            bool client_registered = false;
            for(int i = 1; i <= MAX_CLIENTS; i++) {
                if(fds[i].fd == -1) {
                    fds[i].fd = clientSocket;
                    client_registered = true;
                    m_mtx.lock();
                    m_clients.push_back(std::move(std::make_unique<Client>(clientSocket, clientAddr, i)));
                    m_ids++;
                    m_mtx.unlock();

                    break;
                }
            }

            if(!client_registered) {
                close(clientSocket);
            }
        }

        for(int i = 1; i <= MAX_CLIENTS; i++) {
            if(fds[i].fd != -1 && (fds[i].revents & POLLIN)) {
                messageHandle(m_clients[i - 1]);
            }
        }
    }
}

void Server::messageHandle(const std::unique_ptr<Client>& client) {
    size_t r;
    uint8_t buff[BUFLEN];
    bzero(buff, BUFLEN);

    int clientSocket = client->getSocket();
    r = recv(clientSocket, buff, BUFLEN, 0);

    if(r > 0) {
        if(r < 16) {
            return;
            ;
        }
        this->m_messageHandler(buff, r, client->getId());
    }
}

void Server::messageHandler(const std::unique_ptr<Client> &client) {
    if(m_status != socket_status::ACTIVE) {
        return;
    }

    using namespace std::chrono_literals;

    int clientSocket = client->getSocket();
    size_t r;
    uint8_t buff[BUFLEN];
    bzero(buff, BUFLEN);

    while(m_run.load(std::memory_order_relaxed)) {
        r = recv(clientSocket, buff, BUFLEN, 0);

        if(r > 0) {
            if(r < 16) {
                continue;
            }

            this->m_messageHandler(buff, r, client->getId());
        }

        bzero(buff, BUFLEN);
        std::this_thread::sleep_for(50ms);
    }
}

int Server::connectTo(const std::string& host, int port) {
    int serverSocket;
    sockaddr_in serverAddr;

    serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if(serverSocket == -1) {
        std::runtime_error("Failed to create socket");
    }

    bzero(&serverAddr, sizeof(sockaddr));

    serverAddr.sin_family = AF_INET;
    inet_pton(AF_INET, host.data(), &(serverAddr.sin_addr));
    serverAddr.sin_port = htons(port);

    if(connect(serverSocket, (sockaddr*) &serverAddr, sizeof(sockaddr)) != 0) {
        std::runtime_error("Failed to connect");
    }
    m_mtx.lock();
    auto cl = std::make_unique<Client>(serverSocket, serverAddr, m_ids);
    
    // toDo это не будет работать
    m_clients.push_back(std::move(cl));
    m_messageThreads.push_back(std::make_unique<std::thread>(&Server::messageHandler, this, std::ref(cl)));
    
    m_ids++;
    m_mtx.unlock();

    return 0;
}

void Server::sendDataTo(int id, uint8_t *buffer, size_t n) {
    for (const auto &client : m_clients) {
        if (client->getId() == id) {
            client->sendData(buffer, n);
            break;
        }
    }
}

std::list<int> Server::getClientsId() {
    std::list<int> ids;
    for (const auto &client : m_clients) {
        ids.push_back(client->getId());
    }

    return ids;
}
