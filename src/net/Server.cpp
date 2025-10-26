#include "Server.hpp"

#define BUFLEN 1024
#define MAX_CLIENTS 10

Client::Client(int socket, sockaddr_in addr, int id, std::list<std::unique_ptr<Message>>* msgs) {
    m_sock = socket;
    m_cliaddr = addr;
    m_id = id;
    m_msgs = msgs;
}

Client::~Client() {
    if(m_sock == -1) {
        return;
    }
    close(m_sock);
}

int Client::getSocket() {
    return m_sock;
}

uint16_t Client::getPort() {
    return m_cliaddr.sin_port;
}

uint32_t Client::getHost() {
    return m_cliaddr.sin_addr.s_addr;
}

int Client::getId() {
    return m_id;
}

void Client::sendData(Message* msg) {
    size_t size;
    auto enc = msg->toByte(size);
    send(m_sock, enc.get(), size, 0);
}

Server::Server(int port, std::list<std::unique_ptr<Message>>* msgs, std::mutex* mtx) {
    m_status = socket_status::stop;
    m_sock = -1;
    m_port = port;
    m_ids = 0;
    m_msgs = msgs;
    m_msgMtx = mtx;
}

Server::~Server() {
    stop();
}

void Server::stop() {
    if(m_status == socket_status::stop) {
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

    m_status = socket_status::stop;
}

void Server::start() {
    if(m_status == socket_status::start) {
        return;
    }

    m_sock = socket(AF_INET, SOCK_STREAM, 0);

    if(m_sock == -1) {
        std::cerr << "sock creation error\n";
        m_status = socket_status::sock_err;
        return;
    }

    bzero(&m_servaddr, sizeof(sockaddr));

    m_servaddr.sin_family = AF_INET;
    m_servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    m_servaddr.sin_port = htons(m_port);

    if(bind(m_sock, (sockaddr*) &m_servaddr, sizeof(sockaddr)) != 0) {
        std::cerr << "socket bind failed\n";
        m_status = socket_status::bind_err;
        return;
    }

    if((listen(m_sock, MAX_CLIENTS)) != 0) {
        std::cerr << "Socket listen failed \n";
        m_status = socket_status::listen_err;
        return;
    }

    m_status = socket_status::start;
    m_run.store(true, std::memory_order_relaxed);
    m_acceptThread = std::make_unique<std::thread>(&Server::acceptClients, this);
}

void Server::acceptClients() {
    if(m_status != socket_status::start) {
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

                    auto cl = std::make_shared<Client>(clientSocket, clientAddr, i, m_msgs);

                    m_mtx.lock();
                    m_clients.push_back(cl);
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

void Server::messageHandle(const std::shared_ptr<Client>& client) {
    size_t r;
    uint8_t buff[BUFLEN];
    bzero(buff, BUFLEN);

    char buffer[1024];
    int clientSocket = client->getSocket();
    r = recv(clientSocket, buff, BUFLEN, 0);

    if(r > 0) {
        if(r < 16) {
            return;
            ;
        }

        uint8_t type = buff[9];

        std::unique_ptr<Message> msg;

        if(type == MsgTypes::gBlocks) {
            msg = std::make_unique<GetBlocksMsg>();
        }

        else if(type == MsgTypes::Inv) {
            msg = std::make_unique<InvMsg>();
        }

        else if(type == MsgTypes::gData) {
            msg = std::make_unique<GetDataMsg>();
        }

        else if(type == MsgTypes::sBlock) {
            msg = std::make_unique<BlockMsg>();
        }

        else if(type == MsgTypes::Tx) {
            msg = std::make_unique<TxMsg>();
        }

        else if(type == MsgTypes::noFound) {
            msg = std::make_unique<NoFoundMsg>();
        } else {
            return;
        }

        msg->parse(buff, r);

        msg->setClientId(client->getId());

        m_msgMtx->lock();
        m_msgs->push_back(std::move(msg));
        m_msgMtx->unlock();
    }
}

void Server::messageHandler(std::shared_ptr<Client> client) {
    if(m_status != socket_status::start) {
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

            uint8_t type = buff[9];

            std::unique_ptr<Message> msg;

            if(type == MsgTypes::gBlocks) {
                msg = std::make_unique<GetBlocksMsg>();
            }

            else if(type == MsgTypes::Inv) {
                msg = std::make_unique<InvMsg>();
            }

            else if(type == MsgTypes::gData) {
                msg = std::make_unique<GetDataMsg>();
            }

            else if(type == MsgTypes::sBlock) {
                msg = std::make_unique<BlockMsg>();
            }

            else if(type == MsgTypes::Tx) {
                msg = std::make_unique<TxMsg>();
            }

            else if(type == MsgTypes::noFound) {
                msg = std::make_unique<NoFoundMsg>();
            } else {
                continue;
            }

            msg->parse(buff, r);

            msg->setClientId(client->getId());

            m_msgMtx->lock();
            m_msgs->push_back(std::move(msg));
            m_msgMtx->unlock();
        }

        bzero(buff, BUFLEN);
        std::this_thread::sleep_for(50ms);
    }
}

int Server::connectTo(std::string host, int port) {
    int serverSocket;
    sockaddr_in serverAddr;

    serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if(serverSocket == -1) {
        return -1;
    }

    bzero(&serverAddr, sizeof(sockaddr));

    serverAddr.sin_family = AF_INET;
    inet_pton(AF_INET, host.data(), &(serverAddr.sin_addr));
    serverAddr.sin_port = htons(port);

    if(connect(serverSocket, (sockaddr*) &serverAddr, sizeof(sockaddr)) != 0) {
        std::cerr << "connection error\n";
        return -1;
    }
    m_mtx.lock();
    auto cl = std::make_shared<Client>(serverSocket, serverAddr, m_ids, m_msgs);
    m_clients.push_back(cl);
    m_messageThreads.push_back(std::make_unique<std::thread>(&Server::messageHandler, this, cl));
    m_ids++;
    m_mtx.unlock();

    return 0;
}

void Server::sendDataTo(int id, Message* msg) {
    for(int i = 0; i < m_clients.size(); i++) {
        if(m_clients[i]->getId() == id) {
            m_clients[i]->sendData(msg);
            break;
        }
    }
}

std::list<int> Server::getClientsId() {
    std::list<int> ids;
    for(size_t i = 0; i < m_clients.size(); i++) {
        ids.push_back(m_clients[i]->getId());
    }

    return ids;
}
