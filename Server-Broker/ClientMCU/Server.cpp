#include "../ClientMCU/Server.hpp"
#include "../General/Connection.hpp"
#include "../General/ThreadSafeCout.hpp"
#include <algorithm>

ClientMCU::Server::Server(unsigned short port)
{
    try
    {
        // Initialization black magic.
        this->m_mainSocket = General::Connection::createSocket();
        General::Connection::doBind(this->m_mainSocket, port);
        General::Connection::doListen(this->m_mainSocket);
        this->m_mainThread = std::thread(&ClientMCU::Server::acceptClients, this);
        this->m_freeingThread = std::thread(&ClientMCU::Server::freeFinishedThreads, this);
    }
    catch (const std::exception& error)
    {
        this->stop();
        LOG_ERROR(error.what());
    }
}
ClientMCU::Server::~Server(void)
{
    this->stop(); // To make sure.
}

void ClientMCU::Server::setSensorsData(const General::SensorsData& sensorsData)
{
    std::lock_guard<std::mutex> lock(this->m_getSensorsDataMutex);
    this->m_sensorsData = sensorsData;
}
bool ClientMCU::Server::getServerStatus(void) const
{
	return this->m_keepRunning;
}
void ClientMCU::Server::stop(void)
{
    this->m_keepRunning = false;

    // I make sure first all sub threads are closed.
    General::Connection::closeSocket(this->m_mainSocket);
    if (this->m_mainThread.joinable()) this->m_mainThread.join();
    if (this->m_freeingThread.joinable()) this->m_freeingThread.join();

    // Then I free to touch clients vector as I wish.
    for (auto& client : this->m_clients)
    {
        auto& socket = client.first;
        auto& thread = client.second;

        General::Connection::closeSocket(socket);
        if (thread.joinable()) thread.join();
    }
}

void ClientMCU::Server::acceptClients(void)
{
    try
    {
        while (this->m_keepRunning)
        {
            SOCKET clientSocket = General::Connection::doAccept(this->m_mainSocket);      
            this->addClient(clientSocket);
        }
    }
    catch (const std::exception& error)
    {
        // If server is closing some methods from General::Connection may and probably will throw.
        if (this->m_keepRunning)
        {
            LOG_WARNING(error.what());
        }
    }
}
void ClientMCU::Server::addClient(SOCKET clientSocket)
{
    std::lock_guard<std::mutex> lock(this->m_clientsMutex);
    this->m_clients.emplace_back(clientSocket, std::thread(&ClientMCU::Server::handle, this, clientSocket));
}
void ClientMCU::Server::handle(SOCKET socket)
{
    try
    {
        while (this->m_keepRunning)
        {
            // Get.
            UINT32 buffer[3];
            General::SensorsData tempSensorsData = this->getSensorsData();

            // Set.
            General::SensorsData::serialize(tempSensorsData, buffer);
            General::Connection::sendData(socket, buffer, sizeof(buffer));
        }
    }
    catch (const std::exception& error)
    {
        // If server is closing some methods from General::Connection may and probably will throw.
        if (this->m_keepRunning)
        {
            LOG_WARNING(error.what());
        }
    }
}
void ClientMCU::Server::freeFinishedThreads(void)
{
    while (this->m_keepRunning)
    {
        // The lock must be here and not at the start because other functions will get into starvation mode.
        std::lock_guard<std::mutex> lock(this->m_clientsMutex);
        for (auto it = this->m_clients.begin(); it != this->m_clients.end();)
        {
            if (!it->second.joinable()) it = this->m_clients.erase(it);
            else ++it;
        }
    }
}
General::SensorsData ClientMCU::Server::getSensorsData(void)
{
    std::lock_guard<std::mutex> lock(this->m_getSensorsDataMutex);
    return this->m_sensorsData;
}