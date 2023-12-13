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

enum socket_status{
    sock_err,
    bind_err,
    listen_err,
    connect_err,
    start,
    stop
};

class Client{
public:
    
    Client(int socket, sockaddr_in addr, int id, std::list<Message *> *msgs);
    
    ~Client();
    
    void sendData(Message *msg);
    
    int getSocket();
    
    int getId();
    
    uint16_t getPort();
    
    uint32_t getHost();
    
private:
    int m_id;
    
    int m_sock;
    
    socket_status m_status;
    
    struct sockaddr_in m_cliaddr;
    
    std::list<Message *> *m_msgs;
};

class Server{
public:
    Server(int port, std::list<Message *> *msgs, std::mutex *msx);
    
    ~Server();
    
    void start();
    
    void stop();
    
    int connectTo(std::string ip, int port);
    
    void sendDataTo(int id, Message *msg);
    
    std::list<int> getClientsId();

private:
    void acceptClients();
    
    void messageHandler(Client *client);
    
    int m_port;
    
    int m_sock;
    
    int m_ids;
    
    struct sockaddr_in m_servaddr;
    
    socket_status m_status;
    
    std::atomic<bool> m_run;
    
    std::vector<Client *> m_clients;
    
    std::list<Message *> *m_msgs;
    
    std::thread *m_acceptThread;
    
    std::vector<std::thread *> m_messageThreads;
    
    std::mutex m_mtx;
    
    std::mutex *m_msgMtx;
};

#endif
