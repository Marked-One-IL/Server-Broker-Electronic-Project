#include "../MainMCU/Server.hpp"
#include "../General/ThreadSafeCout.hpp"
#include "../General/Connection.hpp"
#include <stdexcept>
#include "Server.hpp"

MainMCU::Server::Server(unsigned short port)
{
    try
    {
        // Initialization black magic.
        this->m_mainSocket = General::Connection::createSocket();
        General::Connection::doBind(this->m_mainSocket, port);
        General::Connection::doListen(this->m_mainSocket);

        // I gotta make sure I accept the MainMCU before going to the clients.
        // That's why it's not in the handle() thread.
        this->m_clientSocket = General::Connection::doAccept(this->m_mainSocket);
        std::thread handleThread(&MainMCU::Server::handle, this);
        handleThread.detach();
    }
    catch (const std::exception& error)
    {
        this->stop();
        LOG_ERROR(error.what());
    }
}
MainMCU::Server::~Server(void)
{
    stop(); // To make sure.
}

bool MainMCU::Server::getServerStatus(void) const
{
    return this->m_keepRunning;
}
General::SensorsData MainMCU::Server::getSensorsData(void)
{
    std::lock_guard<std::mutex> lock(this->m_getSensorsDataMutex);
    return this->m_sensorsData;
}
void MainMCU::Server::stop(void)
{
    this->m_keepRunning = false;
    General::Connection::closeSocket(this->m_mainSocket);
    General::Connection::closeSocket(this->m_clientSocket);
}

void MainMCU::Server::handle(void)
{
    try
    {
        while (this->m_keepRunning)
        {
            // Get.
            UINT32 buffer[3];
            General::Connection::receiveData(this->m_clientSocket, buffer, sizeof(buffer));

            // Set.
            General::SensorsData tempSensorsData = General::SensorsData::deserialize(buffer);
            this->setSensorsData(tempSensorsData);
        }
    }
    catch (const std::exception& error)
    {
        // If server is closing some methods from General::Connection may and probably will throw.
        if (this->m_keepRunning)
        {
            this->m_keepRunning = false;
            LOG_ERROR(error.what());
        }
    }
}
void MainMCU::Server::setSensorsData(const General::SensorsData& sensorsData)
{
    std::lock_guard<std::mutex> lock(this->m_getSensorsDataMutex);
    this->m_sensorsData = sensorsData;
}