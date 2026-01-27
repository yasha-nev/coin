#include "Client.hpp"

Client::Client(int socket, sockaddr_in addr, ClientID id):
    m_id(id),
    m_sock(socket),
    m_cliaddr(addr) {
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

ClientID Client::getId() const noexcept {
    return m_id;
}

void Client::sendData(uint8_t* buffer, size_t n) {
    send(m_sock, buffer, n, 0);
}
