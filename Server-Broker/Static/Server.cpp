#include "../Static/Server.hpp"
#include "../Global/ThreadSafeCout.hpp"
#include <stdexcept>

Static::Server::Server(const char* ip, unsigned short port)
{
    WSADATA wsa_data {};
    if (WSAStartup(MAKEWORD(2, 2), &wsa_data) != 0)
        throw std::runtime_error("Error at Static::Server::Server(): WSAStartup Failed");
    Global::cout << "Static::Server::Server(): Init WSAStartup" << Global::endl;

    this->m_mainSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (this->m_mainSocket == INVALID_SOCKET)
        throw std::runtime_error("Error at Static::Server::Server(): Init Main Socket Failed");
    Global::cout << "Static::Server::Server(): Init Main Socket" << Global::endl;

    sockaddr_in sa { 0 };
    sa.sin_port = htons(port);
    sa.sin_family = AF_INET;
    sa.sin_addr.s_addr = INADDR_ANY;
    if (bind(this->m_mainSocket, reinterpret_cast<sockaddr*>(&sa), sizeof(sa)) == SOCKET_ERROR)
        throw std::runtime_error("Error at Static::Server::Server(): Binding Failed");
    Global::cout << "Static::Server::Server(): Binded" << Global::endl;
    if (listen(this->m_mainSocket, SOMAXCONN) == SOCKET_ERROR)
        throw std::runtime_error("Error at Static::Server::Server(): Listening Failed");
    Global::cout << "Static::Server::Server(): Listening" << Global::endl;

    this->m_serverThread = std::thread(&Static::Server::handle, this);
}
Static::Server::~Server(void)
{
    Global::cout << "Static::Server::~Server() was called" << Global::endl;
    stop();

    if (this->m_clientSocket != INVALID_SOCKET)
    {
        closesocket(this->m_clientSocket);
        this->m_clientSocket = INVALID_SOCKET;
    }
    if (this->m_mainSocket != INVALID_SOCKET)
    {
        closesocket(this->m_mainSocket);
        this->m_mainSocket = INVALID_SOCKET;
    }

    WSACleanup();
}

bool Static::Server::getServerStatus(void) const
{
    return this->m_keepRunning;
}
Global::SensorsData Static::Server::getSensorsData(void)
{
    std::lock_guard<std::mutex> lock(this->m_getSensorsDataMutex);
    return this->m_sensorsData;
}

void Static::Server::stop(void)
{
    Global::cout << "Static::Server::stop() was called" << Global::endl;

    this->m_keepRunning = false;

    if (this->m_serverThread.joinable())
    {
        if (this->m_mainSocket != INVALID_SOCKET) shutdown(m_mainSocket, SD_BOTH);
        if (this->m_clientSocket != INVALID_SOCKET) shutdown(this->m_clientSocket, SD_BOTH);
        this->m_serverThread.join();
    }
}

void Static::Server::handle(void)
{
    try
    {
        this->m_clientSocket = accept(this->m_mainSocket, nullptr, nullptr);
        if (this->m_clientSocket == INVALID_SOCKET)
            throw std::runtime_error("Error at Static::Server::handle(): Client Accept Failed");
        Global::cout << "Static::Server::handle(): Accepted client" << Global::endl;

        while (this->m_keepRunning)
        {
            Global::SensorsData tempSensorsData;
            this->receiveData(this->m_clientSocket, reinterpret_cast<char*>(&tempSensorsData), sizeof(tempSensorsData));
            this->setSensorsData(tempSensorsData);
            Global::cout << "Static::Server::handle(): Updated Sensors Data" << Global::endl;
        }

        closesocket(this->m_clientSocket);
        this->m_clientSocket = INVALID_SOCKET;
    }
    catch (const std::exception& error)
    {
        Global::cout << error.what() << Global::endl;
        closesocket(this->m_clientSocket);
        this->m_clientSocket = INVALID_SOCKET;
        this->m_keepRunning = false;
    }
}
void Static::Server::setSensorsData(const Global::SensorsData& sensorsData)
{
    std::lock_guard<std::mutex> lock(this->m_getSensorsDataMutex);
    this->m_sensorsData = sensorsData;
}
void Static::Server::receiveData(SOCKET socket, char* buffer, int size)
{
    int received = 0;

    for (std::size_t totalReceived = 0; totalReceived < size; totalReceived += received)
    {
        received = recv(socket, reinterpret_cast<char*>(buffer + totalReceived), static_cast<int>(size - totalReceived), 0);
        if (received == 0) throw std::runtime_error("Error at Static::Server::receiveData(): Client disconnected");
        if (received == SOCKET_ERROR) throw std::runtime_error("Error at Static::Server::receiveData(): Receive failed");
    }
}
