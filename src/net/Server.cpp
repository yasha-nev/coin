#include "Server.hpp"

#define BUFLEN 1024

Server::Server(int port, std::function<void(uint8_t*, size_t, ClientID)> messageHandler):
    m_status(socket_status::DISACTIVE),
    m_port(port),
    m_sock(-1),
    m_messageHandler(messageHandler) {
    m_pollDescriptors.resize(DEFAULT_CLIENTS_COUNT);

    m_pollDescriptors[0].fd = this->m_sock;
    m_pollDescriptors[0].events = POLLIN;

    for(int i = 1; i <= DEFAULT_CLIENTS_COUNT; i++) {
        m_pollDescriptors[i].fd = -1;
        m_pollDescriptors[i].events = POLLIN;
    }
}

Server::~Server() {
    stop();
}

void Server::stop() {
    if(m_status == socket_status::DISACTIVE) {
        return;
    }
    close(m_sock);

    if(m_run.load(std::memory_order_relaxed)) {
        m_run.store(false, std::memory_order_relaxed);
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

    if((listen(m_sock, DEFAULT_CLIENTS_COUNT)) != 0) {
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

    while(m_run.load(std::memory_order_relaxed)) {
        int poll_result = poll(m_pollDescriptors.data(), DEFAULT_CLIENTS_COUNT + 1, 100);
        if(poll_result == -1) {
            break;
        }

        if(m_pollDescriptors[0].revents & POLLIN) {
            sockaddr_in clientAddr;
            int len = sizeof(sockaddr);

            int clientSocket = accept(m_sock, (sockaddr*) &clientAddr, (socklen_t*) &len);
            if(clientSocket == -1) {
                continue;
            }

            bool client_registered = addClient(clientSocket, clientAddr);

            if(!client_registered) {
                close(clientSocket);
            }
        }

        readReceivedMessages();
    }
}

bool Server::addClient(int clientSocket, sockaddr_in clientAddr) {
    bool clientRegistered = false;

    std::lock_guard<std::mutex> lock(m_mtx);
    int id = 0;
    for(size_t i = 0; i < m_pollDescriptors.size(); i++) {
        if(m_pollDescriptors[i].fd == -1) {
            m_pollDescriptors[i].fd = clientSocket;
            m_clients.push_back(std::move(std::make_unique<Client>(clientSocket, clientAddr, id)));
            clientRegistered = true;
            break;
        } else {
            id++;
        }
    }

    return clientRegistered;
}

void Server::readReceivedMessages() {
    size_t r;
    uint8_t buff[BUFLEN];
    bzero(buff, BUFLEN);

    for(size_t i = 1; i <= m_pollDescriptors.size(); i++) {
        if(m_pollDescriptors[i].fd != -1 && (m_pollDescriptors[i].revents & POLLIN)) {
            const auto& client = m_clients[i - 1];
            int clientSocket = client->getSocket();
            r = recv(clientSocket, buff, BUFLEN, 0);

            if(r < 16) {
                continue;
            }

            this->m_messageHandler(buff, r, client->getId());
        }
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

    addClient(serverSocket, serverAddr);

    return 0;
}

void Server::sendDataTo(ClientID id, uint8_t* buffer, size_t n) {
    for(const auto& client: m_clients) {
        if(client->getId() == id) {
            client->sendData(buffer, n);
            break;
        }
    }
}

std::list<ClientID> Server::getClientsId() {
    std::list<ClientID> ids;
    for(const auto& client: m_clients) {
        ids.push_back(client->getId());
    }

    return ids;
}
