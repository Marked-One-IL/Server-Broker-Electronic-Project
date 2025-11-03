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
        std::thread acceptThread(&ClientMCU::Server::acceptClients, this);
        acceptThread.detach();
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

    General::Connection::closeSocket(this->m_mainSocket);
    for (auto& client : this->m_clients)
    {
        General::Connection::closeSocket(client);
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
            this->removeClient(socket);
            LOG_WARNING(error.what());
        }
    }
}
void ClientMCU::Server::addClient(SOCKET clientSocket)
{
    std::lock_guard<std::mutex> lock(this->m_clientsMutex);
    if (not this->m_keepRunning) return; // The server could close but a thread can still slip in.

    std::thread clientThread(&ClientMCU::Server::handle, this, clientSocket);
    clientThread.detach();
    this->m_clients.emplace_back(clientSocket);
}
void ClientMCU::Server::removeClient(SOCKET clientSocket)
{
    std::lock_guard<std::mutex> lock(this->m_clientsMutex);
    if (not this->m_keepRunning) return; // The server could close but a thread can still slip in.

    auto it = std::find(this->m_clients.begin(), this->m_clients.end(), clientSocket);
    if (it == this->m_clients.end()) return;

    General::Connection::closeSocket(*it);
    this->m_clients.erase(it);
}
General::SensorsData ClientMCU::Server::getSensorsData(void)
{
    std::lock_guard<std::mutex> lock(this->m_getSensorsDataMutex);
    return this->m_sensorsData;
}