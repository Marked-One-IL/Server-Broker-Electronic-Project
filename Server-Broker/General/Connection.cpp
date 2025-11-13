#include "../General/Connection.hpp"
#include <stdexcept>
#include <string_view>

static General::Connection init;

SOCKET General::Connection::createSocket(void)
{
    SOCKET tempSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (INVALID_SOCKET == tempSocket) throw std::runtime_error("Could not create self");
    return tempSocket;
}
void General::Connection::doBind(SOCKET self, unsigned short port)
{
    sockaddr_in sa{};
    sa.sin_port = htons(port);
    sa.sin_family = AF_INET;
    sa.sin_addr.s_addr = INADDR_ANY;
    if (SOCKET_ERROR == bind(self, reinterpret_cast<sockaddr*>(&sa), sizeof(sa))) throw std::runtime_error("Could not bind");
}
void General::Connection::doListen(SOCKET self)
{
    if (SOCKET_ERROR == listen(self, SOMAXCONN)) throw std::runtime_error("Could not listen");
}
SOCKET General::Connection::doAccept(SOCKET self)
{
    SOCKET tempSocket = accept(self, nullptr, nullptr);
    if (INVALID_SOCKET == tempSocket) throw std::runtime_error("Could not accept");
    return tempSocket;
}
void General::Connection::closeSocket(SOCKET& self)
{
    if (INVALID_SOCKET != self)
    {
        shutdown(self, SD_BOTH); // Graceful shutdown.
        closesocket(self); // Forced close to avoid block threads from closing.
        self = INVALID_SOCKET;
    }
}

void General::Connection::receiveData(SOCKET self, void* buffer, int size)
{
    int received = 0;
    for (int totalReceived = 0; totalReceived < size; totalReceived += received) 
    {
        received = recv(self, static_cast<char*>(buffer) + totalReceived, size - totalReceived, 0);
        if (SOCKET_ERROR == received or 0 == received) throw std::runtime_error("Client disconnected");
    }
}
void General::Connection::receiveHttp(SOCKET self, std::string& buffer, int maxSize)
{
    int received = 0, totalReceived = 0;
    buffer.resize(maxSize + 1);

    for (totalReceived = 0; totalReceived < maxSize; totalReceived += received)
    {
        received = recv(self, static_cast<char*>(buffer.data()) + totalReceived, maxSize - totalReceived, 0);
        if (SOCKET_ERROR == received or 0 == received) throw std::runtime_error("Client disconnected");

        buffer[totalReceived + received] = '\0';
        if (buffer.find("\r\n\r\n") != std::string::npos) break;
    }

    buffer.resize(totalReceived);
}
void General::Connection::sendData(SOCKET self, const void* buffer, int size)
{
    int sent = 0;
    for (int totalSent = 0; totalSent < size; totalSent += sent) 
    {
        sent = send(self, static_cast<const char*>(buffer) + totalSent, size - totalSent, 0);
        if (SOCKET_ERROR == sent or 0 == sent) throw std::runtime_error("Client disconnected");
    }
}

General::Connection::Connection(void)
{
    WSADATA wsa_data{};
    if (0 != WSAStartup(MAKEWORD(2, 2), &wsa_data)) throw std::runtime_error("WSAStartup Failed");
}
General::Connection::~Connection(void)
{
    WSACleanup();
}